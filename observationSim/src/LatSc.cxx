/**
 * @file LatSc.cxx
 * @brief Implementation of LatSc class.
 * @author J. Chiang
 *
 * $Header$
 */

#include "astro/EarthCoordinate.h"
#include "astro/PointingTransform.h"
#include "astro/GPS.h"

#include "LatSc.h"

namespace observationSim {

astro::SkyDir LatSc::zAxis(double time) {
   HepRotation rotationMatrix = InstrumentToCelestial(time);
   return astro::SkyDir(rotationMatrix(Hep3Vector(0, 0, 1)),
                        astro::SkyDir::EQUATORIAL);
}

astro::SkyDir LatSc::xAxis(double time) {
   HepRotation rotationMatrix = InstrumentToCelestial(time);
   return astro::SkyDir(rotationMatrix(Hep3Vector(1, 0, 0)),
                        astro::SkyDir::EQUATORIAL);
}

double LatSc::EarthLon(double time) {
//   astro::GPS *gps = astro::GPS::instance();
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   return gps->lon();
}

double LatSc::EarthLat(double time) {
//   astro::GPS *gps = astro::GPS::instance();
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   return gps->lat();
}

HepRotation LatSc::InstrumentToCelestial(double time) {
//   astro::GPS *gps = astro::GPS::instance();
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   astro::SkyDir xAxis(gps->RAX(), gps->DECX());
   astro::SkyDir zAxis(gps->RAZ(), gps->DECZ());

   astro::PointingTransform transform(zAxis, xAxis);
   return transform.localToCelestial();
}

int LatSc::inSaa(double time) {
   astro::EarthCoordinate earthCoord( EarthLat(time), EarthLon(time) );
   return earthCoord.insideSAA();
}

} // namespace observationSim
