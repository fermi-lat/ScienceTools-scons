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
   astro::GPS * gps = astro::GPS::instance();
   gps->getPointingCharacteristics(time);
   return gps->lon();
}

double LatSc::EarthLat(double time) {
   astro::GPS * gps = astro::GPS::instance();
   gps->getPointingCharacteristics(time);
   return gps->lat();
}

HepRotation LatSc::InstrumentToCelestial(double time) {
   astro::GPS *gps = astro::GPS::instance();
   gps->getPointingCharacteristics(time);
   astro::SkyDir xAxis(gps->RAX(), gps->DECX());
   astro::SkyDir zAxis(gps->RAZ(), gps->DECZ());

   astro::PointingTransform transform(zAxis, xAxis);
   return transform.localToCelestial();
}

bool LatSc::inSaa(double time) {
   astro::EarthCoordinate earthCoord( EarthLat(time), EarthLon(time) );
   return earthCoord.insideSAA();
}

void LatSc::getScPosition(double time, std::vector<double> & position) {
   Hep3Vector pos = astro::GPS::instance()->position(time);
   position.clear();
// GPS returns the position in units of km, but FT2 wants meters so
// we multiply by 10^3.
   double mperkm(1e3);
   position.push_back(pos.x()*mperkm);
   position.push_back(pos.y()*mperkm);
   position.push_back(pos.z()*mperkm);
}

void LatSc::getZenith(double time, double & ra, double & dec) {
   astro::GPS::instance()->getPointingCharacteristics(time);
   ra = astro::GPS::instance()->RAZenith();
   dec = astro::GPS::instance()->DECZenith();
}

double LatSc::livetimeFrac(double time) const {
   astro::GPS::instance()->getPointingCharacteristics(time);
   return astro::GPS::instance()->livetime_frac();
}

void LatSc::setLivetimeFrac(double frac) {
   astro::GPS::instance()->setLivetime_frac(frac);
}

} // namespace observationSim
