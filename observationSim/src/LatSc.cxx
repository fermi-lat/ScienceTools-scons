/**
 * @file LatSc.cxx
 * @brief Implementation of LatSc class.
 * @author J. Chiang
 *
 * $Header$
 */

#include "astro/EarthCoordinate.h"
#include "FluxSvc/../src/GPS.h"
#include "LatSc.h"

namespace observationSim {

astro::SkyDir LatSc::zAxis() {
   HepRotation rotationMatrix = InstrumentToCelestial();
   return astro::SkyDir(rotationMatrix(Hep3Vector(0, 0, 1)),
                        astro::SkyDir::CELESTIAL);
}

astro::SkyDir LatSc::xAxis() {
   HepRotation rotationMatrix = InstrumentToCelestial();
   return astro::SkyDir(rotationMatrix(Hep3Vector(1, 0, 0)),
                        astro::SkyDir::CELESTIAL);
}

double LatSc::EarthLon() {
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(m_time);
   return gps->lon();
}

double LatSc::EarthLat() {
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(m_time);
   return gps->lat();
}

HepRotation LatSc::InstrumentToCelestial() {
// Get the rotation matrix from instrument to Galactic coordinates
// from GPS.
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(m_time);
   HepRotation glastToGalactic(gps->transformGlastToGalactic(m_time));

// Create astro::SkyDir objects for the instrument axes. 
   astro::SkyDir xAxisCel(glastToGalactic(Hep3Vector(1, 0, 0)),
                          astro::SkyDir::GALACTIC);
   astro::SkyDir yAxisCel(glastToGalactic(Hep3Vector(0, 1, 0)),
                          astro::SkyDir::GALACTIC);
   astro::SkyDir zAxisCel(glastToGalactic(Hep3Vector(0, 0, 1)),
                          astro::SkyDir::GALACTIC);

// Return the desired rotation matrix.
   return HepRotation(xAxisCel(), yAxisCel(), zAxisCel());
}

int LatSc::inSaa() {
   astro::EarthCoordinate earthCoord(EarthLon(), EarthLat());
   return earthCoord.insideSAA();
}

} // namespace observationSim
