/**
 * @file LatProperties.h
 * @brief Class to encapsulate some LAT-specific quantities from FT2.
 * 
 * @author J. Chiang
 *
 * $Header$
 **/

#ifndef astro_LatProperties_h
#define astro_LatProperties_h

namespace astro {

class LatProperties {
public:

   LatProperties() : m_lat_mode(0), m_lat_config(0), m_data_qual(0),
                     m_livetime(1), m_start(0), m_stop(1),
                     m_rock_angle(50) {}

   LatProperties(int lat_mode, int lat_config, int data_qual, double livetime,
                 double start, double stop, double rock_angle) 
      : m_lat_mode(lat_mode), m_lat_config(lat_config), m_data_qual(data_qual),
        m_livetime(livetime), m_start(start), m_stop(stop),
        m_rock_angle(rock_angle) {}

   double livetime_frac() const {
      return m_livetime/(m_stop - m_start);
   }

   int lat_mode() const {
      return m_lat_mode;
   }

   int lat_config() const {
      return m_lat_config;
   }

   int data_qual() const {
      return m_data_qual;
   }

   double livetime() const {
      return m_livetime;
   }
   
   double interval_start() const {
      return m_start;
   }

   double interval_stop() const {
      return m_stop;
   }

   double rock_angle() const {
      return m_rock_angle;
   }

private:

   int m_lat_mode;
   int m_lat_config;
   int m_data_qual;
   double m_livetime;
   double m_start;
   double m_stop;
   double m_rock_angle;

};

} // namespace astro
#endif // astro_LatProperties_h
