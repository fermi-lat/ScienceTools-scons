/** @file SkyDir.h
    @brief declaration of the class SkyDir

   $Header$

*/
#ifndef OrbitModel_SkyDir_H
#define OrbitModel_SkyDir_H


// Include files
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <cmath>
#include <utility> // for pair
#include <string>
#include "astro/SkyProj.h"

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
            EQUATORIAL=1,
            //! a projection, e.g. AIT. For the constructor, the projection parameters must be set properly
            PROJECTION=2 
        } CoordSystem ;

        //! projection types: MER is mercator, AIT, is Hammer-Aitoff...
        typedef enum { CAR, SIN, TAN, ARC, NCP, GLS, MER, AIT, STG, BAD } ProjType; 

        ///Constructors
        ///(l,b) or (Ra, Dec) or projection instanciation
        SkyDir(double param1=0, double param2=0, CoordSystem inputType = EQUATORIAL);

        //! initialize from a vector direction
        SkyDir(Hep3Vector, CoordSystem inputType = EQUATORIAL);

        /** initialize using a projection and coordinates given in that projection
         @param pixelx value of x pixel
         @param pixely value of y pixel
         @param projection  projection to use to obtain world coordinates
         @param galactic if true, the 
         */
        SkyDir(double pixelx, double pixely, const SkyProj& projection, bool galactic=false);


        //! function operator returns the direction
        Hep3Vector& operator () () {return m_dir;}
        const Hep3Vector& operator () ()const  {return m_dir;}
        //! glactic l in degrees
        double l () const;
        //! glactic b in degrees
        double b () const;
        //! equatorial ra in degrees
        double ra () const;
        //! equatorial dec in degrees
        double dec () const;

        //! unit vector (in default equatorial system)
        const Hep3Vector& dir () const {return m_dir;}

        //!to return the opening angle (in radians) between two objects:
        double difference(const SkyDir& other)const;

        /** @brief Routine that gives a projection of the current coordinates 
            @param projection The projection transfomation to apply. 
            @param galactic [false] if true, generate transformation in galactic coords*/

        std::pair<double,double> project(const SkyProj& projection, bool galactic=false) const;

		/** @brief Does a default Hammer-Aitoff projection of the ra and dec */
		std::pair<double,double> project() const;

        /** @brief inverse projection function for reference: units are degrees

     	@param point_x     coordinates of the projected point
	@param point_y          in the projection coordinate system
	@param point_ra    ra and dec (radians) of the point to 
	@param point_dec        be projected        
        */
        static int inverseProjection( double point_x, double point_y, double *point_ra, double *point_dec);

        class Exception; // forward declaration

    private:
        static HepRotation s_equatorialToGalactic;

        Hep3Vector m_dir;
        //  std::pair<double,double> setGalCoordsFromDir() const;
        void setGalCoordsFromDir(double&, double &) const;


    };


} // namespace astro
#endif    // LHCBEVENT_SKYDIR_H
