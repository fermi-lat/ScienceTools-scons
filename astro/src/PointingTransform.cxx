// $Header$

// Include files
#include "astro/PointingTransform.h"

namespace astro {

/** @brief initialize from Z and X directions
    @param inputType SkyDir
    */
	PointingTransform::PointingTransform(SkyDir zDir, SkyDir xDir)
		:m_xDir(xDir),m_zDir(zDir)
	{}
            
	HepRotation PointingTransform::localToGalactic () const{
		const Hep3Vector& xd(m_xDir.dir());
		const Hep3Vector& zd(m_zDir.dir());
		const Hep3Vector& yd( -(m_xDir.dir()).cross( m_zDir.dir() ) );
		HepRotation ret(xd,yd,zd);
		return ret;
	}
			
	SkyDir PointingTransform::gDir(Hep3Vector localDir) const{
		Hep3Vector dir(localDir);
		SkyDir ret(localToGalactic()*dir,SkyDir::CELESTIAL);
		return ret;
	}

} // namespace astro