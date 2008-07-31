/** @file GPS.h
@brief declare class GPS

$Header$
*/
#ifndef ASTRO_GPS_H
#define ASTRO_GPS_H

#include "facilities/Scheduler.h"
#include "facilities/Observer.h"

#include "astro/SkyDir.h"
#include "astro/EarthCoordinate.h"
#include "astro/PointingInfo.h"

#include "CLHEP/Vector/Rotation.h"

#include <string>
#include <stdexcept>
namespace astro {

    // forward declarations
class PointingHistory;
class EarthOrbit;

/** 
* \class GPS
* \brief Models the Global Positoning System for a spacecraft. Handles time, position, and orientation.
* 
Represents the Global Positioning System on-board the spacecraft. An Orbit
object is used to compute the spacecraft's position and pointing characteristics.
Time is tracked through this object, and synchronized with the Scheduler for 
discrete event simulation. 

An expansion factor is provided to allow for acceleration. 

*/
class GPS  
{
public:

    enum CoordSystem { 
        GLAST=0,  //! 0: The original direction is in the GLAST frame already
        LAT = 0,  //! 0; LAT is not really GLAST anymore 
        ZENITH=1, //! 1: rotate from the earth-zenith frame to GLAST
        CELESTIAL=2 //! 2: rotate from the cartesian celestial coordinate system (like a SkyDir)
    };

    enum RockType { 
        NONE,  ///<  No rocking rotation done at all.
        UPDOWN, ///< Satellite will be rocked toward the north pole in the northern hemisphere, opposite in the south.
        SLEWING, ///< (experimental) like UPDOWN, except that rotation at equator happens gradually.
        ONEPERORBIT, ///< LAT rocked northward for one orbit, southward for the next.
        EXPLICIT, ///<  Explicit angles given - this is used only if rotAngles get set.
        POINT, //!  Inertial pointing 
        HISTORY, //! This setting is for using a previously generated pointing database to represent the orbit.
        HISTORY_X_EAST ///< use history for everything but the x-axis, which will be oriented East 
    };

    double	time () const; /// <current time

    double lat()const;  /// < latitude (degrees)
    double lon()const; ///  < longitude (degrees)
    double altitude()const; ///< altitude (km)

    astro::SkyDir zAxisDir()const; ///< spacecraft z-axis direction
    astro::SkyDir xAxisDir()const; ///< spacecraft x-axis direction
    astro::SkyDir zenithDir()const;///< local zenith direction
    CLHEP::Hep3Vector position()const;///< return current position;
    astro::EarthCoordinate earthpos()const;    ///< position in Earth coordinates


    // non-const versions that allow setting the time before retrieving info.
    double lat(double t);  /// < latitude (degrees)
    double lon(double t); ///  < longitude (degrees)
    double altitude(double t); ///< altitude (km)

    astro::SkyDir zAxisDir(double t); ///< spacecraft z-axis direction
    astro::SkyDir xAxisDir(double t); ///< spacecraft x-axis direction
    astro::SkyDir zenithDir(double t);///< local zenith direction
    CLHEP::Hep3Vector position(double t);///< return current position;
    astro::EarthCoordinate earthpos(double t);    ///< position in Earth coordinates


    /// return a rotation matrix for the requested transformation
    CLHEP::HepRotation transformToGlast(double seconds,CoordSystem index);

    /// @brief transform a direction from the given coordinate system to GLAST/LAT coordinates
    /// @param index coordinate system: LAT, ZENITH, or CELESTIAL
    /// @param dir direction to transform
    /// @param met mission elapsed time: default (-1) means use current time
    /// @return the direction in LAT coordinates
    CLHEP::Hep3Vector LATdirection(CoordSystem index,const CLHEP::Hep3Vector& dir, double met=-1);

    /// @brief transform a direction from the LAT system to a SkyDir
    /// @param latdir particle direction to transform 
    /// @param met mission elapsed time: default (-1) means use current time
    /// @return a Sky position, including the sign reversal
    astro::SkyDir toSky(const CLHEP::Hep3Vector& latdir, double met=-1);

    /** @brief stellar aberration: apparent difference in position
       @param sdir actual equatorial direction of object (from a SkyDir, presumably)
       @met   MET (seconds)
       @return the vector difference, to be added or subtracted to the unit vector

       See http://en.wikipedia.org/wiki/Aberration_of_light for a discussion.
    */
    CLHEP::Hep3Vector aberration(const CLHEP::Hep3Vector &sdir,double met=-1);

    /// @brief make a correction, using alignment and/or aberration to a skydir
    /// @param sdir uncorrected direction
    /// @param met [-1] MET seconds: use current value if not specified
    astro::SkyDir correct(const astro::SkyDir& sdir, double met=-1);

    /// expansion of the current orbit
    double      expansion () const; 
    /// sample interval for random orbit distribution
    double     sampleintvl () const; 


    /// pass a specific amount of time
#ifndef SWIG // avoid warning message
    void    pass ( double );
#endif
    /// set the expansion factor for the orbit (-1) = random
    void    expansion ( double );
    /// synchronize w. scheduler
    void    synch ();    
    /// set the sample interval
    void    sampleintvl ( double );

    /// set the direction to point
    void setPointingDirection(const astro::SkyDir& dir);

    /** @brief set the desired pointing history file to use. 
        @param fileName 
        @param offset mission elapsed time for "launch", 
              number to be added to time increments
       @param x_horizontal if true, force x axis to be horizontal, y down

       The file can be either FT1 or an ascii format
       
    */
    void setPointingHistoryFile(std::string fileName, double offset=0, bool x_horizontal=false);
#ifndef SWIG
    /** @class NoHistory
        @brief exception class to be thrown if no history has been loaded
    */
    class NoHistoryError : public std::runtime_error{
    public: 
        NoHistoryError(const std::string& msg): std::runtime_error(msg){}
    };
#endif
    /// access to a const reference for the history. Error if does not exist.
    const astro::PointingHistory& history()const 
#ifndef SWIG // not recognized by swig
        throw(NoHistoryError)
#endif
        ;

    // notification support, managed by facilities/Observer
    void notifyObservers() { m_notification.notify();}
    Subject&    notification() { return m_notification; }

    // static access/destruction
    static GPS*	instance();
    static void     kill ();

    /// set angle to "rock"
    double rockingDegrees(double rockDegrees);

    /// set pointing strategy
    GPS::RockType setRockType(RockType rockType);

    void    time ( double );// set time

    double endTime()const;

    static int test();

    /// @brief set the rotation to be used for alignment
    /// @param r The misalignment matrix, so needs to be inverted to apply
    void setAlignmentRotation(const CLHEP::HepRotation& r);

    /// @brief enable the application of the aberration
    /// @flag [true] value to set
    /// @return the current value
    bool enableAberration(bool flag=true){bool t=m_enableAberration; m_enableAberration=flag; return t;}

protected:
    // singleton - protect ctor/dtor
    GPS();
    virtual ~GPS();

private:
    static GPS* s_instance;
    astro::EarthOrbit* m_earthOrbit; //orbital position object, from the astro package.
    astro::PointingHistory* m_history;

    double m_time;	    // global time
    double m_lastQueriedTime; //the last time that was asked for
    double m_expansion;    // orbit expansion factor

    // notification
    double  m_sampleintvl;  // interval to sample for each pt. in the orbit - to normalize spectra
    Subject  m_notification; 

    double m_rockDegrees; //number of degrees to "rock" the spacecraft, along the local x axis. 
    RockType m_rockType;//current rocking scheme
    
    astro::PointingInfo m_currentPoint;


    astro::SkyDir m_point; ///< set for pointing 

    CLHEP::HepRotation m_alignment; ///< set to apply alignment

    ///! update position, orientaion
    void update(double inputTime);

    bool m_enableAberration;

};
} // namespace
#endif 
