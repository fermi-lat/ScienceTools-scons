/**
 * @file ScData.h
 * @brief Simple data structure to hold ScData data.
 * @author J. Chiang
 * $Header$
 */

#ifndef observationSim_ScData_h
#define observationSim_ScData_h

#include "astro/SkyDir.h"

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
          double lat, const astro::SkyDir &zAxis, const astro::SkyDir &xAxis,
          int inSAA, const std::vector<double> & position) :
      m_time(time), m_RAz(RAz), m_Decz(Decz), m_lon(lon), 
      m_lat(lat), m_zAxis(zAxis), m_xAxis(xAxis), m_inSaa(inSAA),
      m_position(position) {}

   /// Time in seconds (referenced to the zero time of the orbit
   /// calculation in astro::EarthOrbit).
   double time() const {return m_time;}

   /// The RA of the instrument z-axis in degrees.
   double raz() const {return m_RAz;}

   /// The Dec of the instrument z-axis in degrees.
   double decz() const {return m_Decz;}

   /// The Earth longitude of the spacecraft in degrees.
   double lon() const {return m_lon;}

   /// The Earth latitude of the spacecraft in degrees.
   double lat() const {return m_lat;}

   /// The spacecraft z-axis in "Celestial" (J2000?) coordinates.
   astro::SkyDir zAxis() const {return m_zAxis;}

   /// The spacecraft x-axis in "Celestial" (J2000?) coordinates.
   astro::SkyDir xAxis() const {return m_xAxis;}

   /// Flag to indicate if the spacecraft is in the SAA.
   int inSaa() const {return m_inSaa;}

   /// The spacecraft position in geocentric coordinates (km).
   const std::vector<double> & position() const {return m_position;}

private:

   double m_time;
   double m_RAz;
   double m_Decz;
   double m_lon;
   double m_lat;
   astro::SkyDir m_zAxis;
   astro::SkyDir m_xAxis;
   int m_inSaa;
   std::vector<double> m_position;
};

} // namespace observationSim

#endif // observationSim_ScData_h
