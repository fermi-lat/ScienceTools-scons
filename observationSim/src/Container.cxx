/**
 * @file Container.cxx
 * @brief Implementation for Container base class.
 * @author J. Chiang
 * $Header$
 */

#include "FluxSvc/../src/GPS.h"
#include "astro/SkyDir.h"
#include "observationSim/Container.h"

namespace observationSim {

HepRotation Container::glastToCelestial(double time) {
// Get the rotation matrix from instrument to Galactic coordinates
// from GPS.
   GPS *gps = GPS::instance();
   gps->getPointingCharacteristics(time);
   HepRotation glastToGalactic(gps->transformGlastToGalactic(time));

// Create astro::SkyDir objects for the instrument axes. (Much of this
// ugliness could be avoided if astro::SkyDir had a coordinate option
// in the constructor that takes a Hep3Vector as it does for the one
// that takes longitude and latitude.)
   Hep3Vector xAxisGal(glastToGalactic(Hep3Vector(1., 0., 0.)));
   double glon = atan2(xAxisGal.y(), xAxisGal.x())*180./M_PI;
   double glat = asin(xAxisGal.z())*180./M_PI;
   astro::SkyDir xAxisCel(glon, glat, astro::SkyDir::GALACTIC);

   Hep3Vector yAxisGal(glastToGalactic(Hep3Vector(0., 1., 0.)));
   glon = atan2(yAxisGal.y(), yAxisGal.x())*180./M_PI;
   glat = asin(yAxisGal.z())*180./M_PI;
   astro::SkyDir yAxisCel(glon, glat, astro::SkyDir::GALACTIC);

   Hep3Vector zAxisGal(glastToGalactic(Hep3Vector(0., 0., 1.)));
   glon = atan2(zAxisGal.y(), zAxisGal.x())*180./M_PI;
   glat = asin(zAxisGal.z())*180./M_PI;
   astro::SkyDir zAxisCel(glon, glat, astro::SkyDir::GALACTIC);

// Finally, return the desired rotation matrix.

   return HepRotation(xAxisCel(), yAxisCel(), zAxisCel());
}

} // namespace observationSim
