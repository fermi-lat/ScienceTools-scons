//
#ifndef astro_EarthOrbit_H
#define astro_EarthOrbit_H


#include "astro/JulianDate.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/Rotation.h"
namespace astro {
    
/** @class EarthOrbit
* @brief Postition of Earth satellite
* @author G. Tosti original code 
* @author T. Burnett convert to class
* $Id$
    */
    class EarthOrbit   {
    public:
        
        EarthOrbit();
        
        /** 
        * set up for calculation, with orbital parameters currently wired in
        */
        void initialize();
        
        /** Orbit calculation 
        * @param jd the Julian Date for the orbit calculation
        *  @return  The orbit position (in inertial coordinates) in km
        */
        Hep3Vector position(JulianDate jd)const;
        
        /**
        * return the inclination of orbit
        */
        double inclination(){return s_incl;}
        
        /**
        * return the orbital phase, in terms of 'phase since ascending node was passed'
        * @param jd the Julian Date for the orbit calculation
        */
        double phase(JulianDate jd) const;
        
        /** Return the julian date
        * @param seconds number of "elapsed seconds", as exist in FluxSvc
        */ 
        ///this is for interfacing with FluxSvc, which uses "elapsed seconds" as the time parameter.
        JulianDate dateFromSeconds(double seconds) const;
        
    private:
        
    /**
    @brief calculate correction to phase for eccentricity 
        */
        static double Kepler(double MeanAnomaly,double Eccentricity);
        
        double m_M0;
        double m_dMdt;
        double m_Omega0;
        double m_dOmegadt;
        double m_w0;
        double m_dwdt;
        
        double m_a, m_alt;
        
        
        static double s_altitude; //<! nominal altitude (km)
        static double s_incl;     //<! orbit inclination in degrees
        static double s_e;        //<! eccentricity
        
        static double s_radius;   //<! Radius of earth
        
    };
    
}
#endif