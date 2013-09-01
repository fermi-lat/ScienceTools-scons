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
            double energy, double ra, double dec,
            const std::string & irfs="DC2");

   ~Exposure() throw();

   double value(double time) const;

private:

   std::vector<double> m_timeBoundaries;
   std::vector<irfInterface::Irfs *> m_irfs;
   double m_energy;
   astro::SkyDir m_srcDir;
   std::vector<double> m_exposureValues;

   Likelihood::ScData * m_scData;

   void readScData(const std::string & filename);
   void integrateExposure();
   double effArea(double time) const;
};

}

#endif // pyExposure_Exposure_h
