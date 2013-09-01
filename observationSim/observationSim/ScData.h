/**
 * @file ScData.h
 * @brief Simple data structure to hold ScData data.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_ScData_h
#define observationSim_ScData_h

#include "CLHEP/Geometry/Vector3D.h"

namespace observationSim {

/**
 * @class ScData
 * @brief Simple data structure to hold ScData data.
 * @author J. Chiang
 *
 * $Header$
 */

class ScData {

public:

   ScData(double time, double RAz, double Decz, double lon, 
          double lat, int inSAA) :
      m_time(time), m_RAz(RAz), m_Decz(Decz), m_lon(lon), 
      m_lat(lat), m_inSaa(inSAA) {}

   double m_time;
   double m_RAz;
   double m_Decz;
   double m_lon;
   double m_lat;
   int m_inSaa;
};

} // namespace observationSim

#endif // observationSim_ScData_h
