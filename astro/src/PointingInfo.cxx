/** @file PointingInfo.h
    @brief implement class PointingInfo

    $Header$
*/

#include "astro/PointingInfo.h"

using CLHEP::Hep3Vector;

namespace {
   template<typename T>
   inline T linear_interp(T a, T b, double f){
      return (1-f)*a + f*b;
   }
}

namespace astro {

PointingInfo::PointingInfo(const CLHEP::Hep3Vector & position, 
                           const Quaternion & orientation,
                           const EarthCoordinate & earthPos)
   : m_position(position),
     m_q(orientation),
     m_earth(earthPos),
     m_latProperties(LatProperties()) {}

PointingInfo::PointingInfo(const CLHEP::Hep3Vector & position, 
                           const Quaternion & orientation,
                           const EarthCoordinate & earthPos,
                           const LatProperties & latProperties)
   : m_position(position),
     m_q(orientation),
     m_earth(earthPos),
     m_latProperties(latProperties) {}

SkyDir PointingInfo::xAxis()const {
   return m_q.rotate(Hep3Vector(1,0,0));
}

SkyDir PointingInfo::zAxis()const {
   return m_q.rotate(Hep3Vector(0,0,1));
}

SkyDir PointingInfo::zenith()const {
   return SkyDir(m_position);
}

PointingInfo 
PointingInfo::interpolate(const astro::PointingInfo & next,
                          double f, double time) const {
    using CLHEP::Hep3Vector;

    // linear interpolation of position
    Hep3Vector pos1( position()), pos2(next.position());
    double alt1( pos1.mag() )
         , alt2( pos2.mag() )
         , alt( linear_interp(alt1,alt2,f) );
    Hep3Vector position (linear_interp(pos1,pos2,f).unit() * alt );

    // note using the quaternion interpolation (SLERP)
    return PointingInfo(position, m_q.interpolate(next.m_q, f),
                        EarthCoordinate(position,time),
                        m_latProperties);
}

} // namespace astro
