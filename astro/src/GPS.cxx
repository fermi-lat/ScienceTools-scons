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
, m_time(0.) 
, m_lastQueriedTime(-1.)
, m_expansion(1.)    // default expansion:regular orbit for now
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

// access functions that retrive parameters from current state
double        GPS::lat()const{               return m_currentPoint.earthCoord().latitude();}     
double        GPS::lon()const{               return m_currentPoint.earthCoord().longitude();}  	
double        GPS::altitude()const {         return m_currentPoint.earthCoord().altitude();}   
astro::SkyDir GPS::zAxisDir()const{          return m_currentPoint.zAxis();}    
astro::SkyDir GPS::xAxisDir()const{          return m_currentPoint.xAxis();}     
astro::SkyDir GPS::zenithDir()const{         return m_currentPoint.zenith();}
CLHEP::Hep3Vector GPS::position()const{      return m_currentPoint.position();} 
astro::EarthCoordinate GPS::earthpos()const{ return m_currentPoint.earthCoord();}

double	GPS::time ()  const{     return m_time;}

double   GPS::expansion () const{    return m_expansion;}

double GPS::endTime()const{ return m_history==0? 3e8 : m_history->endTime();}

// functions that set the state, then retrieve the given value

double                    GPS::lat(double t){       time(t); return m_currentPoint.earthCoord().latitude();}  
double                    GPS::lon(double t){       time(t); return m_currentPoint.earthCoord().longitude();} 
double                    GPS::altitude(double t){  time(t); return m_currentPoint.earthCoord().altitude();}  
astro::SkyDir             GPS::zAxisDir(double t){  time(t); return m_currentPoint.zAxis();}    
astro::SkyDir             GPS::xAxisDir(double t){  time(t); return m_currentPoint.xAxis();}     
astro::SkyDir             GPS::zenithDir(double t){ time(t); return m_currentPoint.zenith();}
CLHEP::Hep3Vector         GPS::position(double t){  time(t); return m_currentPoint.position();} 
astro::EarthCoordinate    GPS::earthpos(double t){  time(t); return m_currentPoint.earthCoord();}


void GPS::pass ( double t )
{ 
    if (!Scheduler::instance()->running())	{
        time(time() + t);
    }   // cannot pass when scheduler is in control!
}

void GPS::expansion ( double e ){    m_expansion = e; }

void GPS::time ( double t )
{
    // ignore a large request, meant to be invalid, and not expecting anything
    if( t>3e8 ){
        return;
    }

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
    //this function calculates all the relevant position and orientation info

    //decide if the time has changed;  if it has not, we have already calculated all of the following information:
    if(m_lastQueriedTime==inputTime || inputTime<0 )return;
    m_lastQueriedTime=inputTime;


    // if history, just ask the history to interpolate the table, or whatever
    if(m_rockType == HISTORY ){
        m_currentPoint = (*m_history)(inputTime);
        return;
    }

    //Not history: use the default orbit and define useful directions
    astro::JulianDate jDate = m_earthOrbit->dateFromSeconds(inputTime);

    Hep3Vector position( m_earthOrbit->position(jDate) ),
        npole(0,0,1), 
        zenith( position.unit() ),
        east( npole.cross(zenith).unit() );

    astro::EarthCoordinate earthpos(position,inputTime);

    if(m_rockType == POINT){
        // pointing mode
        astro::EarthCoordinate earthpos(position,inputTime);
        Hep3Vector xaxis( npole.cross(m_point()).unit() ); // 
        m_currentPoint = PointingInfo( position, 
            Quaternion(m_point(), xaxis), earthpos );
        return; 
    }

    // Rocking if get here: decide on strategy 
    double rockangle ( m_rockDegrees*M_PI/180 )  // default up
        ,  zenithDec( SkyDir(zenith).dec() );

    if (m_rockType == NONE) {
        rockangle = 0; // zenith pointing
    }else if(m_rockType == UPDOWN){
        if( zenithDec <= 0) rockangle *= -1.;
    }else if(m_rockType == SLEWING){
        //slewing is experimental (not checked by THB)
        if(zenithDec <= 0) rockangle *= -1.;
        if(zenithDec >= -5.5 && zenithDec <= 5.5){
            rockangle -= rockangle*((5.5-fabs(zenithDec))/5.5);
        }
    }else if(m_rockType == ONEPERORBIT){
        double orbitPhase = fmod(m_earthOrbit->phase(jDate), 4*M_PI);
        if(orbitPhase <= 2*M_PI) rockangle *= -1.; 
    }else{
        // EXPLICT is here: don't change angle. axis is E-W.
    }

    // note that final rocking is rotation about E-W, should be orbital plane
    ///@todo define orbital direction for rocking
    m_currentPoint = 
        PointingInfo( position, 
            Quaternion(zenith.rotate(east,rockangle), east), 
            earthpos);
    return;
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
    if ( !in().isNear(out())) ++rc;


    // test rocking
    {
    gps.setRockType(GPS::ONEPERORBIT);
    gps.rockingDegrees(35.);
    double start(0), stop(start+90*60*3), step(60*10);  // will interpolate two intervals
    cout << "\nRocking test:\ntime\tlat\tlon\traz\tdecz" << endl;
    for( double time=start; time<stop; time+=step){
        gps.time(time);  // set the time
        cout << time << "\t"
            << gps.lat() << "\t" 
            << gps.lon() << "\t" 
            << gps.zAxisDir().ra()  << "\t"
            << gps.zAxisDir().dec() << "\t"
            << endl;
    }    
    }
    // test reading and interpolating an ascii file
    const char * package_root(::getenv("ASTROROOT") );
#if 0
    std::string history(std::string(package_root)+"/src/test/history_test.txt");
#else
    std::string history("Y:/common/DC2/history/pointing_history.txt");

#endif
    std::cout << "Reading history file " << history << std::endl;
    gps.setPointingHistoryFile(history);

    double start(gps.endTime()-100), stop(start+61), step(5);  // will interpolate two intervals
    cout << "\nRead history file test\ntime\tlat\tlon\traz\tdecz\trax\tdecz\trazen\tdeczen" << endl;
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
