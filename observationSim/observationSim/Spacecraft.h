/**
 * @file Spacecraft.h
 * @brief Declaration of Spacecraft base class.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef observationSim_Spacecraft_h
#define observationSim_Spacecraft_h

#include "astro/SkyDir.h"

namespace observationSim {

/**
 * @class Spacecraft
 *
 * @brief An abstract base class that defines the interface for
 * objects that provide information on spacecraft position and
 * attitude.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Spacecraft {

public:

   virtual ~Spacecraft() {}

   /// Spacecraft z-axis in J2000 coordinates.
   virtual astro::SkyDir zAxis(double time) = 0;

   /// Spacecraft x-axis in J2000 coordinates.
   virtual astro::SkyDir xAxis(double time) = 0;

   /// Earth longitude in degrees.
   virtual double EarthLon(double time) = 0;

   /// Earth latitude in degrees.
   virtual double EarthLat(double time) = 0;

   /// Rotation matrix from instrument to J2000 coordinates
   virtual HepRotation InstrumentToCelestial(double time) = 0;

   /// Unity if in SAA, zero otherwise.
   virtual int inSaa(double time) = 0;

};

} // namespace observationSim

#endif // observationSim_Spacecraft_h
