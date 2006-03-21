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

    /** @brief transform a local to a celestial direction
    */
    CLHEP::HepRotation PointingTransform::localToCelestial () const{
        const CLHEP::Hep3Vector& xd(m_xDir.dir());
        const CLHEP::Hep3Vector& zd(m_zDir.dir());
        const CLHEP::Hep3Vector& yd( (m_zDir.dir()).cross( m_xDir.dir() ) );
        CLHEP::HepRotation ret(xd,yd,zd);
        return ret;
    }

    /** @brief get the celestial direction from the local one
    */
    SkyDir PointingTransform::gDir(CLHEP::Hep3Vector localDir) const{
        CLHEP::Hep3Vector dir(localDir);
        SkyDir ret(localToCelestial()*dir,SkyDir::EQUATORIAL);
        return ret;
    }

} // namespace astro
