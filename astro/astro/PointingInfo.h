/** @file PointingInfo.h
    @brief declare class PointingInfo

    $Header$
*/
#ifndef ASTRO_POINTINGINFO_H
#define ASTRO_POINTINGINFO_H

#include "astro/SkyDir.h"
#include "astro/EarthCoordinate.h"
#include "astro/Quaternion.h"
#include "astro/LatProperties.h"

namespace astro{

class PointingInfo {
public:
    /** ctor sets everyting
        @param position position
        @param orientation
        @param earthPos
    */
    PointingInfo(const CLHEP::Hep3Vector& position, 
                 const astro::Quaternion& orientation,
                 const astro::EarthCoordinate& earthPos);

#ifndef SWIG   
    PointingInfo(const CLHEP::Hep3Vector& position, 
                 const astro::Quaternion& orientation,
                 const astro::EarthCoordinate& earthPos,
                 const LatProperties & latProperties);
#endif

    /// default ctor
   PointingInfo() {}

   ~PointingInfo() {}

   // accessors
   astro::SkyDir xAxis()const;
   astro::SkyDir zAxis()const;
   astro::SkyDir zenith()const;
   const CLHEP::Hep3Vector& position()const{return m_position;}
   const astro::EarthCoordinate& earthCoord()const{return m_earth;}
   const astro::Quaternion& quaternion() const {return m_q;}

#ifndef SWIG
   const LatProperties & latProperties() const {
      return m_latProperties;
   }
#endif

   ///! equivalent rotation
   CLHEP::HepRotation rotation()const{return m_q.rotation();}

   ///! iterpolation
   PointingInfo interpolate(const astro::PointingInfo & next,
                            double fraction, double time) const;

private:

   CLHEP::Hep3Vector m_position; ///< position (km)
   Quaternion m_q; ///< orientaion of the SC: xaxis direction
   EarthCoordinate m_earth; ///< Earth position: (lat, lon)
   LatProperties m_latProperties;
};

}

#endif
