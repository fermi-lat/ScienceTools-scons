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
        enum CoordSystem { 
            GALACTIC=0,  //!  fixed direction with respect to the galactic coordinate system (l,b)
                CELESTIAL=1, EQUATORIAL=1 //! fixed direction with respect to the celestial coordinate system (ra,dec) in the J2000 epoch.
        };

        typedef enum { CAR, SIN, TAN, ARC, NCP, GLS, MER, AIT, STG } ProjType; 

        ///Constructors
        ///(l,b) or (Ra, Dec) instantiation
        SkyDir(double param1=0, double param2=0, CoordSystem inputType = CELESTIAL);
        SkyDir(Hep3Vector, CoordSystem inputType = CELESTIAL);
        
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
        void setProjection(const float ref_ra, const float ref_dec,
                const ProjType projType, const float myRef_x, const float myRef_y, 
                const float myScale_x, const float myScale_y, const float rot,
                const bool use_lb);
        // General Projection Function
        std::pair<double,double> project() const;
    private:
        static HepRotation s_celestialToGalactic;

        void initProjection(void);
        
        Hep3Vector m_dir;
	//  std::pair<double,double> setGalCoordsFromDir() const;
	void setGalCoordsFromDir(double&, double &) const;

   bool m_project_lb;  // If true, project uses l, b coords instead of RA, DEC
   float m_refRA;  // Projection Center RA
   float m_refDEC; // Projection Center DEC
   ProjType m_projType; // Projection Type.  Valid values are CAR, SIN, 
                        // TAN, ARC, NCP, GLS, MER, AIT, STG
   float m_refX; // Projection Output Center X
   float m_refY; // Projection Output Center Y
   float m_scaleX; // Projection X Scaling 1/degrees
   float m_scaleY; // Projection Y Scaling 1/degrees
   float m_rot;   // Projection Rotation Angle
        
    };
    
    
} // namespace astro
#endif    // LHCBEVENT_SKYDIR_H
