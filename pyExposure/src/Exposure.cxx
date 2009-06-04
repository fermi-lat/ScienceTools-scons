/**
 * @file Exposure.cxx
 * @brief LAT effective area, integrated over time bins.
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <utility>

#include "st_facilities/Util.h"

#include "irfInterface/IrfsFactory.h"
#include "irfLoader/Loader.h"

#include "Likelihood/LikeExposure.h"
#include "Likelihood/ScData.h"

#include "pyExposure/Exposure.h"

namespace pyExposure {

Exposure::Exposure(const std::string & scDataFile,
                   const std::vector<double> & timeBoundaries,
                   const std::vector< std::pair<double, double> > & gtis,
                   const std::vector<double> & energies, 
                   double ra, double dec, double radius,
                   const std::string & irfs) 
   : m_timeBoundaries(timeBoundaries), m_gtis(gtis), m_energies(energies),
     m_srcDir(astro::SkyDir(ra, dec)), m_radius(radius), m_scData(0) {
   irfLoader::Loader::go();
   const std::vector<std::string> & 
      irfNames(irfLoader::Loader::respIds().find(irfs)->second);
   irfInterface::IrfsFactory & factory(*irfInterface::IrfsFactory::instance());
   for (size_t i = 0; i < irfNames.size(); i++) {
      m_irfs.push_back(factory.create(irfNames.at(i)));
   }
   m_scData = new Likelihood::ScData();
   readScData(scDataFile);
   integrateExposure();
}

Exposure::~Exposure() throw() {
   delete m_scData;
   for (size_t i = 0; i < m_irfs.size(); i++) {
      delete m_irfs.at(i);
   }
}
   
double Exposure::value(double time, double energy) const {
   int indx = std::upper_bound(m_timeBoundaries.begin(), 
                               m_timeBoundaries.end(), time) 
      - m_timeBoundaries.begin() - 1;
   int k = std::upper_bound(m_energies.begin(), m_energies.end(), energy)
      - m_energies.begin() - 1;
   double expVal1 = m_exposureValues.at(indx).at(k);
   double expVal2 = m_exposureValues.at(indx).at(k+1);
   double expVal(0);
   if (expVal1 > 0 && expVal2 > 0) {
      expVal = expVal1*std::exp(std::log(energy/m_energies.at(k))
                                /std::log(m_energies.at(k+1)
                                          /m_energies.at(k))
                                *std::log(expVal2/expVal1));
   } else {
      expVal = (std::log(energy/m_energies.at(k))
                /std::log(m_energies.at(k+1)/m_energies.at(k))
                *(expVal2 - expVal1) + expVal1);
   }
   return std::max(0., expVal);
}

void Exposure::readScData(const std::string & scDataFile) {
   double tmin(*std::min_element(m_timeBoundaries.begin(), 
                                 m_timeBoundaries.end()));
   double tmax(*std::max_element(m_timeBoundaries.begin(), 
                                 m_timeBoundaries.end()));
// Add some padding to ensure the interval covering the end time
// boundary is included.
   size_t npts(m_timeBoundaries.size());
   tmax += std::max(2*(m_timeBoundaries.back() - m_timeBoundaries.at(npts-2)),
                    60.);
   std::vector<std::string> scFiles;
   st_facilities::Util::resolve_fits_files(scDataFile, scFiles);
   std::vector<std::string>::const_iterator scIt = scFiles.begin();
   bool clear(true);
   for ( ; scIt != scFiles.end(); scIt++) {
      st_facilities::Util::file_ok(*scIt);
      m_scData->readData(*scIt, tmin, tmax, clear);
      clear = false;
   }
}

void Exposure::integrateExposure() {
   m_exposureValues.resize(m_timeBoundaries.size()-1);
   for (size_t j(0); j < m_exposureValues.size(); j++) {
      m_exposureValues.at(j).resize(m_energies.size(), 0);
      std::vector< std::pair<double, double> > timeCuts;
      timeCuts.push_back(std::make_pair(m_timeBoundaries.at(j),
                                        m_timeBoundaries.at(j+1)));
      size_t imin, imax;
      try {
         imin = m_scData->time_index(m_timeBoundaries.at(j));
         imax = m_scData->time_index(m_timeBoundaries.at(j+1)) + 1;
      } catch (std::runtime_error &) {
         imin = 0;
         imax = m_scData->numIntervals() - 1;
      }
      imax = std::min(imax, m_scData->numIntervals() - 1);
      for (size_t i(imin); i < imax + 1; i++) {
         double tstart(m_scData->start(i));
         double tstop(m_scData->stop(i));
         double fraction(0);
         if (Likelihood::LikeExposure::
             acceptInterval(tstart, tstop, timeCuts, m_gtis, fraction)) {
            for (size_t k(0); k < m_energies.size(); k++) {
               m_exposureValues.at(j).at(k) += 
                  (effArea(tstart, m_energies.at(k))
                   + effArea(tstop, m_energies.at(k)))/2.
                  *m_scData->livetime(i)*fraction;
            }
         }
      }
   }
}

double Exposure::effArea(double time, double energy) const {
   astro::SkyDir zAxis = m_scData->zAxis(time);
   astro::SkyDir xAxis = m_scData->xAxis(time);
   double theta(m_srcDir.difference(zAxis)*180./M_PI);
   double phi(0);
   
   double my_effArea(0);
   for (size_t i = 0; i < m_irfs.size(); i++) {
      irfInterface::IAeff * aeff = m_irfs.at(i)->aeff();
      double aperture(1);
      if (m_radius < 180.) {
         irfInterface::IPsf * psf = m_irfs.at(i)->psf();
         aperture = psf->angularIntegral(energy, theta, phi, m_radius);
      }
      my_effArea += aeff->value(energy, m_srcDir, zAxis, xAxis)*aperture;
   }
   return my_effArea;
}

} // namespace pyExposure
