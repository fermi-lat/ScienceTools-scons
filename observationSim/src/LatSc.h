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
 * @brief Provide spacecraft attitude and orbital position informatio
 * for the LAT.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class LatSc : public Spacecraft {

public:

   LatSc(double time) : m_time(time) {}

   virtual ~LatSc() {}

   virtual astro::SkyDir zAxis();
   virtual astro::SkyDir xAxis();

   virtual double EarthLon();
   virtual double EarthLat();

   virtual HepRotation InstrumentToCelestial();

   virtual int inSaa();

private:

   double m_time;

};

} // namespace observationSim

#endif // observationSim_LatSc_h
