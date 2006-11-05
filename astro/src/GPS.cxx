/** @file GPS.cxx
 @brief  implementation of the GPS class.

 $Id$
*/
#include "astro/GPS.h"

#include "astro/PointingHistory.h"
#include "astro/EarthOrbit.h"


#include "astro/Quaternion.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace astro;
using namespace CLHEP;

GPS*	GPS::s_instance = 0;

GPS::GPS() 
: m_earthOrbit(new astro::EarthOrbit)
, m_history(0)
, m_expansion(1.)    // default expansion:regular orbit for now
, m_time(0.) 
, m_endTime(0)
, m_lastQueriedTime(-1.)
, m_sampleintvl(1.) // notification interval for clients
, m_rockDegrees(0), m_rockType(NONE) 
{   
    update(0);
}


GPS::~GPS ()
{ delete m_history;
}//delete m_orbit; }


void GPS::synch ()
{
    static bool first=true;
    bool changed=  false;
    static double  last_time = time();

    if (Scheduler::instance()->running()) {
        time( Scheduler::instance()->elapsed_time() );
        changed = true; // maybe have threshold before nofitying?
    }

    // If elapsed time exceeds interval then update
    if ((time() - last_time) > m_sampleintvl) {
        last_time = time();
        changed = true;    
    }

    // notify observers if changed (or first time thru)
    if( changed || first) notifyObservers();
    first=false;

}

double        GPS::lat()const{   return m_currentPoint.earthCoord().latitude();}     
double        GPS::lon()const{   return m_currentPoint.earthCoord().longitude();}  	
double        GPS::altitude()const {    return m_currentPoint.earthCoord().altitude();}   
astro::SkyDir GPS::zAxisDir()const{    return m_currentPoint.zAxis();}    
astro::SkyDir GPS::xAxisDir()const{    return m_currentPoint.xAxis();}     
astro::SkyDir GPS::zenithDir()const{    return m_currentPoint.zenith();}
CLHEP::Hep3Vector GPS::position()const{    return m_currentPoint.position();} 
astro::EarthCoordinate GPS::earthpos()const{    return m_currentPoint.earthCoord();}

double	GPS::time ()  const{     return m_time;}

double   GPS::expansion () const{    return m_expansion;}

void GPS::pass ( double t )
{ 
    if (!Scheduler::instance()->running())	{
        time(time() + t);
    }   // cannot pass when scheduler is in control!
}

void GPS::expansion ( double e ){    m_expansion = e; }

void GPS::time ( double t )
{
    m_time = t;
    update(t);
}

GPS*	GPS::instance() 
{ return (s_instance != 0) ? s_instance : (s_instance = new GPS()); }

void GPS::kill()
{
    delete s_instance;
    s_instance = 0;
}

void    GPS::sampleintvl ( double s ){    m_sampleintvl = s;}

double  GPS::sampleintvl () const{    return m_sampleintvl;}

void GPS::setPointingDirection( const astro::SkyDir& dir){
    m_point = dir;
    m_rockType = POINT;
    m_lastQueriedTime=-1; // make sure recalculate stuff
}

/// set the desired pointing history file to use:
void GPS::setPointingHistoryFile(std::string fileName, double  offset){
    m_rockType = HISTORY;
    m_history = new PointingHistory(fileName, offset);
}

double GPS::rockingDegrees(double rockDegrees){
        double ret=m_rockDegrees;
        m_rockDegrees = rockDegrees;
        return ret;}

GPS::RockType GPS::setRockType(RockType rockType){
    RockType ret (m_rockType);
    m_rockType = rockType;
    return ret;
}

CLHEP::HepRotation GPS::transformToGlast(double seconds, CoordSystem index){
    ///Purpose:  return the rotation to transfrom a vector in a given system to
    ///the S/C satellite system.  

    ///Input:  Current time, and a "coordinate system" index:
    ///0: The original direction is in the GLAST frame already
    ///1: rotate from the local zenith frame to GLAST
    ///2: rotate from the celestial coordinate system (like a SkyDir)

    ///Output:  3x3 rocking-angle transformation matrix.

    update(seconds);
    HepRotation trans; // default identity

    switch (index) {

        case GLAST: 
            //do nothing - we are already in the GLAST frame.
            break;

        case ZENITH: { 
            // earth-zenith to GLAST - just the rocking rotation.
            // first form rotation from local zenith to celestial
            // start with matrix that transforms from celestial to GLAST
            trans= m_currentPoint.rotation().inverse();

            // now form a matrix that transforms from zenith to celestial
            Hep3Vector 
                zenith( zenithDir()() ), 
                north(0,0,1),
                east( north.cross(zenith).unit() );
            HepRotation zenith_to_cel(east, zenith.cross(east), zenith);

            // return the product, zenith->celestial->GLAST
            trans = trans* zenith_to_cel;
            }
            break;

        case CELESTIAL:

            //SkyDir to SC is inverse of SC to SkyDir
            trans= m_currentPoint.rotation().inverse();
            break;

        default:
            throw std::invalid_argument("unexpected index for GPS::transformToGlast");
    }

    return trans;
}


void GPS::update(double inputTime){
    //this function calculates all the relevant satellite data. Note that the rocking is done here as well, 
    //so that pointing characteristics come out right.
    using namespace astro;

    //decide if the time has changed;  if it has not, we have already calculated all of the following information:
    if(m_lastQueriedTime==inputTime || inputTime<0 )return;

    //if not, set the last time to this one:
    m_lastQueriedTime=inputTime;

    //and then get the appropriate julian date:
    astro::JulianDate jDate = m_earthOrbit->dateFromSeconds(inputTime);

    if(m_rockType == POINT){
        // pointing mode
        Hep3Vector position( m_earthOrbit->position(jDate) );
        astro::EarthCoordinate earthpos(position,inputTime);
        Hep3Vector npole(0,0,1);
        Hep3Vector xaxis( npole.cross(m_point()).unit() ); // 
        m_currentPoint = PointingInfo( position, Quaternion(m_point(), xaxis), earthpos );

        return; // all now is set.
    }
    if(m_rockType == HISTORY ){
        m_currentPoint = (*m_history)(inputTime);

    }else{

        if( m_rockType != NONE && m_rockType!=EXPLICIT ) throw(std::invalid_argument("Rocking not implemented"));
        // NONE - use built-in earth orbit, zenith pointing

        Hep3Vector 
            position( m_earthOrbit->position(jDate) ),
            npole(0,0,1), 
            zenith( position.unit() ),
            east( npole.cross(zenith).unit() );

         double rockangle( m_rockDegrees*M_PI/180);

        m_currentPoint = 
            PointingInfo( position, 
                Quaternion(zenith.rotate(east,rockangle), east), 
                EarthCoordinate(position,inputTime) );
        
    }
#if 0  ///@todo still lots of stuff to try to clean up, convert 
    SkyDir dirZ(lZ,bZ,SkyDir::GALACTIC);
    SkyDir dirX(raX,decX);
    //before rotation, the z axis points along the zenith:
    SkyDir dirZenith(dirZ.dir());
    //also, before Rotation, set the characteristics of the zenith x-direction:

    if(m_rockType != HISTORY || inputTime >= m_endTime){
        //rotate the x direction so that the x direction points along the orbital direction.
        dirX().rotate(dirZ.dir() , inclination*cos(orbitPhase));
    }

    // now, we want to find the proper transformation for the rocking angles:
    //HepRotation rockRot(Hep3Vector(0,0,1).cross(dirZ.dir()) , m_rockNorth);    
    //and apply the transformation to dirZ and dirX:
    m_rockNorth = m_rockDegrees*M_PI/180;
    //here's where we decide how much to rock about the x axis.  this depends on the 
    //rocking mode.
    if(m_rockType == NONE){
        m_rockNorth = 0.;
    }else if(m_rockType == UPDOWN){
        if(m_DECZenith <= 0) m_rockNorth *= -1.;
    }else if(m_rockType == SLEWING){
        //slewing is experimental
        if(m_DECZenith <= 0) m_rockNorth *= -1.;
        if(m_DECZenith >= -5.5 && m_DECZenith <= 5.5){
            m_rockNorth -= m_rockNorth*((5.5-fabs(m_DECZenith))/5.5);
        }
    }else if(m_rockType == ONEPERORBIT){
        orbitPhase = fmod(orbitPhase, 2.*CLHEP::twopi);  // TU The above says M_2PI so I assume this is valid
        if(orbitPhase <= CLHEP::twopi) m_rockNorth *= -1.;  // TU convert to local constants
    }else{
    }

#endif
}




int GPS::test()
{
    using namespace astro;
    using namespace std;
    int rc(0);
    GPS& gps = *GPS::instance();
    {
    // default: zenith pointing
    gps.time(1000);
    if( ! gps.zAxisDir()().isNear(gps.zenithDir()())) ++rc;
    HepRotation Rzen = gps.transformToGlast(1000,GPS::ZENITH);
    Hep3Vector localZenith(0,0,1),
     testz ( Rzen* localZenith );
    if( !testz.isNear(localZenith) ) ++rc;
    }
    // test setting zenith angle
    {
    gps.setRockType(GPS::EXPLICIT);
    double rock(20.);
    gps.rockingDegrees(rock);
    gps.time(2000);
    if(  gps.zAxisDir()().isNear(gps.zenithDir()())) ++rc; // should not be near
    HepRotation Rzen = gps.transformToGlast(2000,GPS::ZENITH);
    Hep3Vector localZenith(0,0,1),
     testz ( Rzen* localZenith );
    double dot(testz.dot(localZenith) ), cs(cos(rock*M_PI/180));
    if( fabs(dot-cs) >1e-10  ) ++rc;
    }

    // test pointing
    double in_ra(10), in_dec(-10);
    SkyDir in(in_ra, in_dec);
    gps.setPointingDirection( in );
    gps.time(0);
    SkyDir out(gps.zAxisDir()); 
    double out_ra(out.ra()), out_dec(out.dec());
    if ( !in().isNear(out())) ++rc;


    // test reading and interpolating an ascii file
    const char * package_root(::getenv("ASTROROOT") );
    gps.setPointingHistoryFile(std::string(package_root)+"/src/test/history_test.txt");

    double start(900), stop(start+61), step(5);  // will interpolate two intervals
    cout << "time\tlat\tlon\traz\tdecz\trax\tdecz\trazen\tdeczen" << endl;
    for( double time=start; time<stop; time+=step){
        gps.time(time);  // set the time
        cout << time << "\t"
            << gps.lat() << "\t" 
            << gps.lon() << "\t" 
            << gps.zAxisDir().ra()  << "\t"
            << gps.zAxisDir().dec() << "\t"
            << gps.xAxisDir().ra()  << "\t"
            << gps.xAxisDir().dec() << "\t"
            << gps.zenithDir().ra()    << "\t"
            << gps.zenithDir().dec()   << "\t"
            << endl;
    }

    return rc;
}
