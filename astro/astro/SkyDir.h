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
        SkyDir(Hep3Vector, CoordSystem inputType = EQUATORIAL);


        ///return methods
        Hep3Vector& operator () () {return m_dir;}
        double l () const;
        double b () const;
        double ra () const;
        double dec () const;
        const Hep3Vector& dir () const {return m_dir;}

        //!to return the opening angle (in radians) between two objects:
        double difference(const SkyDir& other)const;

        //hammer-aitoff equal-area projection.
        std::pair<double,double> hammerAitoff()const;

        // Set Projection Attributes
        static void setProjection( float ref_ra,  float ref_dec,
             ProjType projType,  float myRef_x,  float myRef_y, 
             float myScale_x,  float myScale_y,  float rot,
             bool use_lb);

        /// General Projection Function: return projection, given an object
        std::pair<double,double> project() const;

        // inverse for reference
        static int inverseProjection( float point_x, float point_y, float *point_ra, float *point_dec);

    private:
        static HepRotation s_equatorialToGalactic;

        void initProjection(void);

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
