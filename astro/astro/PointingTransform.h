// $Header$
#ifndef OrbitModel_PointingTransform_H
#define OrbitModel_PointingTransform_H


// Include files
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "astro/SkyDir.h"
#include <cmath>
#include <utility> // for pair

namespace astro {
    
    
/** @class PointingTransform
* @brief Describe an absolute direction
* @author S. Robinson 
* <br>$Id$
*
    */
    class PointingTransform
    {
    public:
        ///Constructors
        ///Z,X axis instantiation
		PointingTransform(SkyDir zdir, SkyDir xdir);
        
        ///return methods
        Hep3Vector& operator () () {return m_dir;}
        HepRotation localToGalactic () const;
		SkyDir gDir(Hep3Vector localDir) const;
        
    private:
		SkyDir m_xDir,m_zDir;
		Hep3Vector m_dir;
    };
    
    
} // namespace astro
#endif    // LHCBEVENT_PointingTransform_H
