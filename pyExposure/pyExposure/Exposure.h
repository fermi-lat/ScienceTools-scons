/**
 * @file Exposure
 * @brief LAT effective area, integrated over time bins, to a specific
 * point on the sky.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef pyExposure_Exposure_h
#define pyExposure_Exposure_h

#include <string>
#include <vector>

#include "astro/SkyDir.h"

namespace irfInterface {
   class Irfs;
}

namespace Likelihood {
   class ScData;
}

namespace pyExposure {

/**
 * @class Exposure
 * @brief LAT effective area, integrated over time bins, to a specific
 * point on the sky.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Exposure {

public:

   Exposure(const std::string & scDataFile, 
            const std::vector<double> & timeBoundaries,
            const std::vector<double> & energies, 
            double ra, double dec, double radius, 
            const std::string & irfs="DC2");

   ~Exposure() throw();

   double value(double time, double energy) const;

   const std::vector<double> & timeBoundaries() const {
      return m_timeBoundaries;
   }

   const std::vector<double> & energies() const {
      return m_energies;
   }

   const std::vector< std::vector<double> > & values() const {
      return m_exposureValues;
   }

private:

   std::vector<double> m_timeBoundaries;
   std::vector<irfInterface::Irfs *> m_irfs;
   std::vector<double> m_energies;
   astro::SkyDir m_srcDir;
   double m_radius;
   std::vector< std::vector<double> > m_exposureValues;

   Likelihood::ScData * m_scData;

   void readScData(const std::string & filename);
   void integrateExposure();
   double effArea(double time, double energy) const;
};

}

#endif // pyExposure_Exposure_h
