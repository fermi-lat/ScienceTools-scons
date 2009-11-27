/** @file SkyDir.h
@brief declaration of the class SkyDir


*/
#ifndef astro_SkyDir_H
#define astro_SkyDir_H


// Include files
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <cmath>
#include <utility> // for pair
#include <string>
#include "astro/SkyProj.h"

/*
> Date: Tue, 9 Aug 2005 11:18:24 -0400 (EDT)
> From: James Peachey <James.Peachey-1@nasa.gov>
> 
> Rootcint and Cint are notoriously bad with macros, especially when 
> used in unusual ways, such as using HepStd to switch between using std 
> and not using it. For what it's worth, I found that by adding the 
> following to astro/SkyDir.h, the rootcint command you sent was able to create 
> the dictionary (whether it works is another matter).
*/
#ifdef __MAKECINT__
#include <iostream>
 namespace HepStd {
   typedef std::ostream ostream;
 }
#endif

namespace astro {


    /** @class SkyDir
    * @brief Describe an absolute direction
    * @author S. Robinson 
    * <br>$Id$
    *
    * Note that units associated with sky coordinates (ra, dec, l, b) are consistently in degrees
    */

    class SkyDir
    {
    public:
        typedef enum  { 
            //!  fixed direction with respect to the galactic coordinate system (l,b)
            GALACTIC=0,  
            //! fixed direction with respect to the equatorial coordinate system (ra,dec) in the J2000 epoch.
            EQUATORIAL=1
        } CoordSystem ;


        ///Constructors
        ///(l,b) or (Ra, Dec) or projection instanciation
        SkyDir(double param1=0, double param2=0, CoordSystem inputType = EQUATORIAL);

        //! initialize from a vector direction
        SkyDir(CLHEP::Hep3Vector, CoordSystem inputType = EQUATORIAL);

        /** initialize using a projection and coordinates given in that projection
        @param pixelx value of x pixel
        @param pixely value of y pixel
        @param projection  projection to use to obtain world coordinates
        @param check  if false and the pixels are invalid, throw an exception. if true, make the direction invalid
        */
        SkyDir(double pixelx, double pixely, const SkyProj& projection, bool check=false);


        //! function operator returns the direction
        CLHEP::Hep3Vector& operator () () {return m_dir;}
        const CLHEP::Hep3Vector& operator () ()const  {return m_dir;}
        //! glactic l in degrees
        double l () const;
        //! glactic b in degrees
        double b () const;
        //! equatorial ra in degrees
        double ra () const;
        //! equatorial dec in degrees
        double dec () const;

        //! unit vector (in default equatorial system)
        const CLHEP::Hep3Vector& dir () const {return m_dir;}

        //!to return the opening angle (in radians) between two objects:
        double difference(const SkyDir& other)const;

        /** @brief Routine that returns a projection

        @param projection The projection transfomation to apply. 
        @return (pixelx,pixely)

        Note that if the projection specificies galactic coordinates, the 
        projection will be applied to (l,b) rather than (ra,dec)
        */
        std::pair<double,double> project(const SkyProj& projection) const;

        //! check for validity: m_dir.z is made gt 1 if invalid
        bool isValid()const{ return m_dir.z()<1.0; }

        /** @brief determine coordinates with respect to zenith

        @param zenithDir the direction defining the zenith
        @return the azimuth and latitude in degrees. Note that the heading is 90-azimuth mod 360
        */
        std::pair<double, double> zenithCoords(const astro::SkyDir& zenithDir)const;

      
        astro::SkyDir cross(const astro::SkyDir & other) const;
        //! return cross product (as a SkyDir) with respect to another direction

        double dot(const astro::SkyDir & other) const;
        //! return dot product with respect to other directin
 

    private:
        static CLHEP::HepRotation s_equatorialToGalactic;

        CLHEP::Hep3Vector m_dir;
        void setGalCoordsFromDir(double&, double &) const;

    };


} // namespace astro
#endif    // LHCBEVENT_SKYDIR_H
