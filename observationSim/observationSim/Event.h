/**
 * @file Event.h
 * @brief Simple data structure to hold Event data.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_Event_h
#define observationSim_Event_h

#include "CLHEP/Geometry/Vector3D.h"

namespace observationSim {

/**
 * @class Event
 * @brief Simple data structure to hold Event data.
 * @author J. Chiang
 *
 * $Header$
 */

class Event {

public:

   Event(double time, double energy, const Hep3Vector &appDir, 
         const Hep3Vector &srcDir) :
      m_time(time), m_energy(energy), m_appDir(appDir), m_srcDir(srcDir) {}

   double m_time;
   double m_energy;
   Hep3Vector m_appDir;
   Hep3Vector m_srcDir;

};

} // namespace observationSim

#endif // observationSim_Event_h
