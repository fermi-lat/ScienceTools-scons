// $Header$
#ifndef OrbitModel_SkyDir_H
#define OrbitModel_SkyDir_H


// Include files
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include <cmath>
#include <utility> // for pair

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
            GALACTIC=0,  //!  fixed direction with respect to the galactic coordinate system (l,b)
            EQUATORIAL=1, //! fixed direction with respect to the celestial coordinate system (ra,dec) in the J2000 epoch.
            PROJECTION=2 //! a projection, e.g. AIT
        } CoordSystem ;

        typedef enum { CAR, SIN, TAN, ARC, NCP, GLS, MER, AIT, STG } ProjType; 

        ///Constructors
        ///(l,b) or (Ra, Dec) instantiation
        SkyDir(double param1=0, double param2=0, CoordSystem inputType = EQUATORIAL);

        //! initialize from a vector direction
        SkyDir(Hep3Vector, CoordSystem inputType = EQUATORIAL);


        //! function operator returns the direction
        Hep3Vector& operator () () {return m_dir;}
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

        //!hammer-aitoff equal-area projection, scaled to fit in box with x=+-2, y=+-1.
        std::pair<double,double> hammerAitoff()const;

        /** @brief Set Projection Attributes

        @param ref_ra      point of reference for the
        @param ref_dec     projection (projection center)
        @param projType    projection type
        @param myRef_x     coordinates of the projection center
        @param myRef_y           in the projection coordinate system
        @param myScale_x   x scale of the projection coordinate system (1/degrees) at the projection center
        @param myScale_y   y scale of the projection coordinate
        system (1/degrees) at the projection center
        @param rot         rotation of the projection coordinate system
        0 = x-axis parallel to lines of equal DEC
        M_PI = x-axis parallel to lines of equal RA
        @param use_lb      If set to true, the projection gets the current galactic
        coordinates instead of ra/dec.  Also it is implicit that 
        ref_ra and ref_dec now refer to the projection center's 
        coordinates in terms of l, b instead of ra, dec.
        */

        static void setProjection( float ref_ra,  float ref_dec,
            ProjType projType,  float myRef_x,  float myRef_y, 
            float myScale_x,  float myScale_y,  float rot,
            bool use_lb);

        /// General Projection Function: return projection for the object, given an object. 
        /// depends on static projection attributes
        std::pair<double,double> project() const;

        /** @brief inverse projection function for reference: units are degrees

     	@param point_x     coordinates of the projected point
	@param point_y          in the projection coordinate system
	@param point_ra    ra and dec of the point to 
	@param point_dec        be projected        
        */
        static int inverseProjection( double point_x, double point_y, double *point_ra, double *point_dec);

    private:
        static HepRotation s_equatorialToGalactic;

        Hep3Vector m_dir;
        //  std::pair<double,double> setGalCoordsFromDir() const;
        void setGalCoordsFromDir(double&, double &) const;

        static bool  s_project_lb;  // If true, project uses l, b coords instead of RA, DEC
        static float s_refRA;  // Projection Center RA
        static float s_refDEC; // Projection Center DEC
        static ProjType s_projType; // Projection Type.  Valid values are CAR, SIN, 
        // TAN, ARC, NCP, GLS, MER, AIT, STG
        static float s_refX; // Projection Output Center X
        static float s_refY; // Projection Output Center Y
        static float s_scaleX; // Projection X Scaling 1/degrees
        static float s_scaleY; // Projection Y Scaling 1/degrees
        static float s_rot;   // Projection Rotation Angle

    };


} // namespace astro
#endif    // LHCBEVENT_SKYDIR_H
