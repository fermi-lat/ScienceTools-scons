/**
 * @file LatSc.h
 * @brief Declaration for the LAT spacecraft object.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef observationSim_LatSc_h
#define observationSim_LatSc_h

#include "observationSim/Spacecraft.h"

namespace observationSim {

/**
 * @class LatSc
 *
 * @brief Provide spacecraft attitude and orbital position information
 * for the LAT.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class LatSc : public Spacecraft {

public:

   LatSc() {}

   virtual ~LatSc() {}

   virtual astro::SkyDir zAxis(double time);
   virtual astro::SkyDir xAxis(double time);

   virtual double EarthLon(double time);
   virtual double EarthLat(double time);

   virtual HepRotation InstrumentToCelestial(double time);

   virtual int inSaa(double time);

   virtual void getScPosition(double time, std::vector<double> & scPosition);

   virtual void getZenith(double time, double & ra, double & dec);
};

} // namespace observationSim

#endif // observationSim_LatSc_h
