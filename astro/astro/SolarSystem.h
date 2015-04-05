/** @file SolarSystem.h
@brief definition of class SolarSystem 


$Header$
*/
#ifndef astro_SolarSystem_H
#define astro_SolarSystem_H

#include "astro/JulianDate.h"
#include "astro/SkyDir.h"
#include <stdexcept>


namespace astro {

    /** @class SolarSystem
    * @brief position of various solarsystem entities
    * @author T. Burnett 
    *
    * This is a wrapper around the JPL ephemeris
    */

    class SolarSystem  {

    public:
        /**
        * index of body that corresponds to the elements in dpleph class
        */
//        typedef enum Body { MERCURY=1,
       enum Body { MERCURY=1,
            VENUS=2, EARTH=3, MARS=4,
            JUPITER=5, SATURN=6, URANUS=7,
            NEPTUNE=8, PLUTO=9, MOON=10, SUN=11};

        /** @brief simple constructor that initializes
        * @param body Body targ
        */
        SolarSystem(Body body=SUN);
        ~SolarSystem();
#ifndef SWIG
        /** @class BadDate
            @brief exception class inheriting from invalid_argumetn if Julian date is invalid

        */
        class BadDate: public std::invalid_argument{
        public:
            BadDate( const std::string&msg): std::invalid_argument(msg){}
        };
#endif
        /**
        * @brief return the SkyDir at date, with respect to position (in km) from center of Earth 
        */
        SkyDir direction(JulianDate jd, const CLHEP::Hep3Vector& position)const ;

        /**
        * @brief return the SkyDir at date
        */
        SkyDir direction(JulianDate jd)const throw( SolarSystem::BadDate);

		/**
        * @brief return the distance of the body from Earth at date in lightseconds
        */
        double distance(JulianDate jd)const throw( SolarSystem::BadDate);

        /**
        * @brief return the distance vector to solar system barycenter at date in lightseconds
        */
        CLHEP::Hep3Vector getBarycenter(JulianDate jd)const throw( SolarSystem::BadDate);

        /**
        * @brief return a distance vector to the sun at date in lightseconds
        */
        CLHEP::Hep3Vector getSolarVector(JulianDate jd)const throw( SolarSystem::BadDate);

        /*
        * @brief returns the distance vector between two bodies: target and center at date in lightseconds
        */
        static CLHEP::Hep3Vector vector(Body targ, Body cent, JulianDate jd) throw( SolarSystem::BadDate);


    private:
        //! setup the jpl ephemeris database if needed, return jd in correct form
        static double * jplSetup(JulianDate jd) ;
        Body m_body;       ///< target body
    };

} // namespace astro

#endif
