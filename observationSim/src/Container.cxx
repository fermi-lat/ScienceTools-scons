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

} // namespace observationSim
