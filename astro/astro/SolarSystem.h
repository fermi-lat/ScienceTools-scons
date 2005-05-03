/** @file SolarSystem.h
@brief definition of class SolarSystem 


$Header$
*/
#ifndef astro_SolarSystem_H
#define astro_SolarSystem_H

#include "astro/JulianDate.h"
#include "astro/SkyDir.h"

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
        typedef enum Body { MERCURY=1,
            VENUS=2, EARTH=3, MARS=4,
            JUPITER=5, SATURN=6, URANUS=7,
            NEPTUNE=8, PLUTO=9, MOON=10, SUN=11};

        /** @brief simple constructor that initializes
        * @param body Body targ
        */
        SolarSystem(Body body=SUN);
        ~SolarSystem();

        /**
        * @brief return the SkyDir at date 
        */
        SkyDir direction(JulianDate jd)const ;

        /**
        * @brief return the distance of the body from Earth at date in lightseconds
        */
        double distance(JulianDate jd)const;

        /**
        * @brief return the distance vector to solar system barycenter at date in lightseconds
        */
        Hep3Vector getBarycenter(JulianDate jd)const;

        /**
        * @brief return a distance vector to the sun at date in lightseconds
        */
        Hep3Vector getSolarVector(JulianDate jd)const;

        /*
        * @brief returns the distance vector between two bodies: target and center at date in lightseconds
        */
        static Hep3Vector vector(Body targ, Body cent, JulianDate jd);


    private:
        //! setup the jpl ephemeris database if needed, return jd in correct form
        static double * jplSetup(JulianDate jd);
        Body m_body;       ///< target body
    };

} // namespace astro

#endif
