/**
 * @file Roi.cxx
 * @brief Implementation for region-of-interest class that describes
 * an acceptance cone about some direction.
 * @author J. Chiang
 *
 * $Header$
 */

#include "CLHEP/Geometry/Vector3D.h"

#include "astro/SkyDir.h"

#include "flux/EventSource.h"

#include "observationSim/../src/LatSc.h"
#include "observationSim/Roi.h"

namespace observationSim {

bool Roi::operator()(EventSource *event) {

   double time = event->time();
   Hep3Vector launchDir = event->launchDir();

// We need the source direction in J2000 coordinates here.
// Unfortunately, because EventSource objects are so closely tied to
// the LAT geometry as embodied in the various FluxSvc and astro
// classes (e.g., GPS, EarthOrbit, EarthCoordinate), launchDir is
// accessible only as a Hep3Vector in LAT coordinates.  Therefore, we
// are forced to cheat and use a LatSc object explicitly in order to
// recover the source direction in J2000 coordinates.
   LatSc latSpacecraft;
   HepRotation rotMatrix = latSpacecraft.InstrumentToCelestial(time);
   astro::SkyDir sourceDir(rotMatrix(launchDir), astro::SkyDir::EQUATORIAL);

   double separation = sourceDir.difference(m_center)*180./M_PI;

   return separation <= m_coneAngle;
}

} // namespace observationSim
