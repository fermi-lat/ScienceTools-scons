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

namespace {
double saa_lat_max(double lng) {

   long i, nl=5;
 
   double saa_long[] = {-88., -55., -32., -7., 50.};
   double saa_lat[] = {-12., -0.1, -0.1, -12., -25.};

//   hunt(saa_long-1, nl, lng, &i);
   for (i = 0; i < nl-1 ; i++) {
      if (lng >= saa_long[i] && lng < saa_long[i+1]) break;
   }
   i++;

   return (lng - saa_long[i-1])/(saa_long[i] - saa_long[i-1])
      *(saa_lat[i] - saa_lat[i-1]) + saa_lat[i-1];
}
} // unnamed namespace

namespace observationSim {

astro::SkyDir LatSc::zAxis(double time) {
   HepRotation rotationMatrix = InstrumentToCelestial(time);
   return astro::SkyDir(rotationMatrix(Hep3Vector(0, 0, 1)),
                        astro::SkyDir::CELESTIAL);
}

astro::SkyDir LatSc::xAxis(double time) {
   HepRotation rotationMatrix = InstrumentToCelestial(time);
   return astro::SkyDir(rotationMatrix(Hep3Vector(1, 0, 0)),
                        astro::SkyDir::CELESTIAL);
}

double LatSc::EarthLon(double time) {
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   return gps->lon();
}

double LatSc::EarthLat(double time) {
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   return gps->lat();
}

HepRotation LatSc::InstrumentToCelestial(double time) {
// Get the rotation matrix from instrument to Galactic coordinates
// from GPS.
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   HepRotation glastToGalactic(gps->transformGlastToGalactic(time));

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

int LatSc::inSaa(double time) {
// astro::EarthCoordinate::insideSAA() is all fouled up.
//    astro::EarthCoordinate earthCoord(EarthLon(time), EarthLat(time));
//    return earthCoord.insideSAA();

// So use instead the method from obs_sim::in_saa(...)
   int nl = 5;
   double saa_long[] = {-88., -55., -32., -7., 50.};
   double saa_lat_min = -50.;

   double lon = EarthLon(time);
   double lat = EarthLat(time);

   int in_saa;
   if (lon > saa_long[0] && lon < saa_long[nl-1]) {
      in_saa = (lat <= ::saa_lat_max(lon)) && (lat >= saa_lat_min);
   } else {
      in_saa = 0;
   }

   return in_saa;
}

} // namespace observationSim
