/** @file GPS.cxx
@brief  implementation of the GPS class.

$Id$
*/
#include "astro/GPS.h"

#include "astro/PointingHistory.h"
#include "astro/EarthOrbit.h"
#include "astro/SolarSystem.h"

#include "astro/Quaternion.h"
#include "facilities/commonUtilities.h"
#include <iomanip>
#include <sstream>
#include <cassert>
#include <stdlib.h>

using namespace astro;
using namespace CLHEP;

namespace {
    const double D2R = M_PI/180.;
}

GPS*	GPS::s_instance = 0;

GPS::GPS() 
: m_earthOrbit(new astro::EarthOrbit)
, m_history(0)
, m_time(0.) 
, m_lastQueriedTime(-1.)
, m_expansion(1.)    // default expansion:regular orbit for now
, m_sampleintvl(1.) // notification interval for clients
, m_rockDegrees(0), m_rockType(NONE) 
, m_enableAberration(false)
{   
    update(0);
}


GPS::~GPS ()
{ delete m_history;
}//delete m_orbit; }


const astro::PointingHistory& GPS::history()const throw(GPS::NoHistoryError)
{   if( m_history==0) throw GPS::NoHistoryError("GPS:: no history has been loaded");
    return *m_history;
}


void GPS::synch ()
{
    static bool first=true;
    if( m_lastQueriedTime <0 ) first =true; // make sure that notification will occur
    bool changed=  false;
    static double  last_time = time();

    if (Scheduler::instance()->running()) {
        time( Scheduler::instance()->elapsed_time() );
        changed = true; // maybe have threshold before nofitying?
    }

    // If elapsed time exceeds interval then update
    if ((time() - last_time) > m_sampleintvl  || time()< last_time) {
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
    if( t>3e10 ){
        return;
    }

    m_time = t; // set the new time
    update(t);  // update orientation, etc.
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
void GPS::setPointingHistoryFile(std::string fileName, double  offset, bool x_east){
    m_rockType = x_east? HISTORY_X_EAST : HISTORY;
    m_history = new PointingHistory(fileName, offset);
    m_lastQueriedTime=-1; // make sure recalculate stuff

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
            // this wires it to be looking South
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
            trans *= m_alignment;
            break;

        default:
            throw std::invalid_argument("unexpected index for GPS::transformToGlast");
    }

    return trans;
}

CLHEP::Hep3Vector GPS::aberration(const CLHEP::Hep3Vector& pvec, double met) {
    static double seconds_per_day(86400);
    static double cob(20.49552/3600 * D2R); // constant of aberration in radians
    Hep3Vector enp(SkyDir(270,90-23.439281)());  // ecliptic northpole 
    if( met ==-1) { // use current time
        met = time();
    }
    JulianDate jd = met/seconds_per_day + JulianDate::missionStart(); //m_earthOrbit->dateFromSeconds(met);
    Hep3Vector sol = SolarSystem().getSolarVector(jd).unit(); //direction to Sun
    double check( sol * enp ); // direction to sun should be perpendicular
    assert( fabs(check)<1e-4); // verify that current sun direction is perpendicular

    Hep3Vector v( cob* sol.cross(enp).unit() ); //vector in direction of orbit, with v/c magnitude
    Hep3Vector axis( v.cross(pvec) );  // axis of rotation
    HepRotation rot(axis, axis.mag() ); // rotation matrix from apparent to actual
    Hep3Vector result( rot*pvec - pvec); // will return difference
    return result;  
}

CLHEP::Hep3Vector GPS::LATdirection(CoordSystem index,const CLHEP::Hep3Vector& dir, double met)
{
    CLHEP::Hep3Vector result(dir);
    if( met != -1)update( met);
    switch( index){
        case LAT: 
            //do nothing - we are already in the GLAST frame.
            break;
        case ZENITH: 
            { 
                // earth-zenith to GLAST - just the rocking rotation.
                // first form rotation from local zenith to celestial
                // start with matrix that transforms from celestial to GLAST
                HepRotation trans( m_currentPoint.rotation().inverse() );

                // now form a matrix that transforms from zenith to celestial
                // this wires it to be looking South
                Hep3Vector 
                    zenith( zenithDir()() ), 
                    north(0,0,1),
                    east( north.cross(zenith).unit() );
                HepRotation zenith_to_cel(east, zenith.cross(east), zenith);

                // return the product, zenith->celestial->GLAST
                result = trans* zenith_to_cel * result;
            }
            break;
        case CELESTIAL:
            {
                // for this case we need to consider both misalignment and aberration
                // since both are small, we don't worry about second-order
                // assume that the direction is toward the source!
                if( m_enableAberration)  result -= aberration(dir, met);
                result = -(m_alignment.inverse() *  m_currentPoint.rotation().inverse() *   result);
            }
            break; 

        default:
            throw std::invalid_argument("unexpected index for GPS::LATdirection");

    };
    return result;
}

astro::SkyDir GPS::toSky(const CLHEP::Hep3Vector& latdir, double met)
{
    if( met != -1)update( met);

    // apply alignment correction, then rotate (and reverse direction) to get a sky location
    CLHEP::Hep3Vector tdir( - (m_currentPoint.rotation() * m_alignment * latdir) );
    // if enabled, apply aberratin correction
    if( m_enableAberration) tdir += aberration(tdir, met);
    return astro::SkyDir(tdir);
}

astro::SkyDir GPS::correct(const astro::SkyDir& sdir, double met)
{
    if( met != -1)update( met);
    const HepRotation& R(m_currentPoint.rotation()); // rotation
    Hep3Vector tdir( sdir() )     // initial direction from skydir
        , latdir( R.inverse()*tdir ) // in LAT coordinates
        , newdir( R * m_alignment * latdir); // back to sky, after LAT rotation
    
    if( m_enableAberration) newdir += aberration(newdir, met);
    return SkyDir(newdir);
}

void GPS::setAlignmentRotation(const CLHEP::HepRotation& r)
{
    m_alignment=r.inverse(); // note saved as inverse
}


void GPS::update(double inputTime){
    //this function calculates all the relevant position and orientation info

    //decide if the time has changed;  if it has not, we have already calculated all of the following information:
    if(m_lastQueriedTime==inputTime || inputTime<0 )return;
    m_lastQueriedTime=inputTime;


    // if history, just ask the history to interpolate the table, or whatever
    if(m_rockType == HISTORY ||  m_rockType == HISTORY_X_EAST ){
        m_currentPoint = (*m_history)(inputTime);
        if( m_rockType == HISTORY_X_EAST) {
           // recreate the pointing guy but with same orientation of x axis as
            // is done for the explicit pointing, in horizontal direction
            astro::EarthCoordinate earth (m_currentPoint.earthCoord());
            CLHEP::Hep3Vector pos (m_currentPoint.position() )
                ,vertical(pos.unit())
                ,zAxis(m_currentPoint.zAxis()())
                ,horizontal(vertical.cross(zAxis).unit());
             m_currentPoint =  PointingInfo(pos, Quaternion(zAxis, horizontal), earth);
        }
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
        Hep3Vector xaxis( npole.cross(m_point()).unit() ); 
        m_currentPoint = PointingInfo( position, 
            Quaternion(m_point(), xaxis), earthpos );
        return; 
    }

    // Rocking if get here: decide on strategy 
    double rockangle ( m_rockDegrees*D2R )  // default up
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
        double dot(testz.dot(localZenith) ), cs(cos(rock*D2R));
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
   // std::string history(std::string(package_root)+"/src/test/history_test.txt");
    std::string history(facilities::commonUtilities::joinPath(facilities::commonUtilities::getDataPath("astro"), "history_test.txt"));

    std::cout << "Reading history file " << history << std::endl;
    gps.setPointingHistoryFile(history);
    const astro::PointingHistory& h = gps.history(); // get the history object

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
    // test aberration
    SkyDir npole(0,90);
    CLHEP::Hep3Vector t(gps.aberration(npole(), 0) );
    double test( t.mag() - 1e-4);

    if( fabs(test)>2e-6)++rc ;// expect within 1% of the total

    CLHEP::Hep3Vector t2(gps.aberration(SkyDir(270,66.55)(), 0) );
    test = ( t2.mag() - 1e-4);

    // test transformation without, then with the aberration
    {
    astro::SkyDir npole(0,90);
    CLHEP::Hep3Vector latdir( gps.LATdirection(GPS::CELESTIAL, npole()));
    astro::SkyDir back( gps.toSky(latdir) ); 
    double check( npole.difference(back));
    if( check> 1e-9) ++rc;
    }
    gps.enableAberration();
    {
    astro::SkyDir npole(0,90);
    CLHEP::Hep3Vector latdir( gps.LATdirection(GPS::CELESTIAL, npole()));
    astro::SkyDir back( gps.toSky(latdir) ); 
    double check( npole.difference(back));
    if( check> 1e-9) ++rc;

    }
    // check exception
    try{
        cout << "\n trying time that is not in the range...\n";
        gps.time(1e6);
    }catch(const PointingHistory::TimeRangeError& x){
        cout << " caught expected exception " << x.what() << std::endl;
        return rc;
    }
    // should not get here
    ++rc;
    return rc;
}
