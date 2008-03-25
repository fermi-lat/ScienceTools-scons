/**
 * @file EventContainer.cxx
 * @brief Implementation for the class that keeps track of events and
 * when they get written to a FITS file.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <cstdlib>

#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Vector3D.h"

using CLHEP::RandFlat;
using CLHEP::Hep3Vector;
using CLHEP::HepRotation;

#include "astro/SkyDir.h"
#include "astro/GPS.h"

#include "fitsGen/Ft1File.h"

#include "flux/EventSource.h"
#include "flux/Spectrum.h"

#include "st_facilities/FitsUtil.h"

#include "irfInterface/Irfs.h"

#include "dataSubselector/Cuts.h"
#include "dataSubselector/Gti.h"

#include "observationSim/EventContainer.h"
#include "observationSim/Spacecraft.h"

namespace {
   double my_acos(double mu) {
      if (mu > 1) {
         return 0;
      } else if (mu < -1) {
         return M_PI;
      } else {
         return acos(mu);
      }
   }

   irfInterface::Irfs* drawRespPtr(std::vector<irfInterface::Irfs*> &respPtrs,
                                   double area, double energy, 
                                   astro::SkyDir &sourceDir,
                                   astro::SkyDir &zAxis,
                                   astro::SkyDir &xAxis, 
                                   double time) {
   
// Build a vector of effective area accumulated over the vector
// of response object pointers.
//
// First, fill a vector with the individual values.
      std::vector<double> effAreas(respPtrs.size());
      std::vector<double>::iterator eaIt = effAreas.begin();
      std::vector<irfInterface::Irfs *>::iterator respIt = respPtrs.begin();
      while (eaIt != effAreas.end() && respIt != respPtrs.end()) {
         *eaIt = (*respIt)->aeff()->value(energy, sourceDir, zAxis, xAxis,
                                          time);
         eaIt++;
         respIt++;
      }

// Compute the cumulative distribution.
      std::partial_sum(effAreas.begin(), effAreas.end(), effAreas.begin());

// The total effective area.
      double effAreaTot(effAreas.back());

// Generate a random deviate from the interval [0, area) to ascertain
// which response object to use.
      double xi = RandFlat::shoot()*area;

      if (xi < effAreaTot) {
// Success. Find the appropriate response functions.
         eaIt = std::lower_bound(effAreas.begin(), effAreas.end(), xi);
         int indx = eaIt - effAreas.begin();
         return respPtrs[indx];
      } else {
// Do not accept this event.
         return 0;
      }
   }

} // unnamed namespace

namespace observationSim {

EventContainer::~EventContainer() {
   if (m_events.size() > 0) {
      writeEvents(m_stopTime);
   }
}

void EventContainer::init() {
   m_events.clear();
}

bool EventContainer::addEvent(EventSource * event, 
                              std::vector<irfInterface::Irfs *> & respPtrs, 
                              Spacecraft * spacecraft,
                              bool flush) {
   std::string particleType = event->particleName();
   double time = event->time();
   double energy = event->energy();
   Hep3Vector launchDir = event->launchDir();

   double arg = launchDir.z();
   double flux_theta = ::my_acos(arg);
   double flux_phi = atan2(launchDir.y(), launchDir.x());
   if (flux_phi < 0) {
      flux_phi += 2.*M_PI;
   }

   HepRotation rotMatrix = spacecraft->InstrumentToCelestial(time);
   astro::SkyDir sourceDir(rotMatrix(-launchDir), astro::SkyDir::EQUATORIAL);

   astro::SkyDir zAxis = spacecraft->zAxis(time);
   astro::SkyDir xAxis = spacecraft->xAxis(time);

   std::string srcName(event->name());
   setEventId(srcName, event->code());

   m_srcSummaries[srcName].incidentNum += 1;
   if (respPtrs.empty()) {
      m_events.push_back( Event(time, energy, 
                                sourceDir, sourceDir, zAxis, xAxis,
                                ScZenith(time), 0, energy, flux_theta,
                                flux_phi, m_srcSummaries[srcName].id) );
      if (flush || m_events.size() >= m_maxNumEntries) {
         writeEvents();
      }
      return true;
   }

   irfInterface::Irfs *respPtr;

// Apply the acceptance criteria.
   bool accepted(false);
   if ( RandFlat::shoot() < m_prob
        && (respPtr = ::drawRespPtr(respPtrs, event->totalArea()*1e4, 
                                    energy, sourceDir, zAxis, xAxis, time))
        && !spacecraft->inSaa(time) 
        && RandFlat::shoot() < spacecraft->livetimeFrac(time) ) {

      astro::SkyDir appDir 
         = respPtr->psf()->appDir(energy, sourceDir, zAxis, xAxis, time);
      double appEnergy(energy);
      if (m_applyEdisp) {
         appEnergy =
            respPtr->edisp()->appEnergy(energy, sourceDir, zAxis, xAxis, time);
      }

      std::map<std::string, double> evtParams;
      evtParams["ENERGY"] = appEnergy;
      evtParams["RA"] = appDir.ra();
      evtParams["DEC"] = appDir.dec();
      if (m_cuts == 0 || m_cuts->accept(evtParams)) {
         m_srcSummaries[srcName].acceptedNum += 1;
         if (m_events.size() > 0 && m_events.back().time() == time) {
            throw std::runtime_error("EventContainer::addEvent:\n"
                                     "zero time interval encountered.");
         }
         m_events.push_back( Event(time, appEnergy, 
                                   appDir, sourceDir, zAxis, xAxis,
                                   ScZenith(time), respPtr->irfID(), 
                                   energy, flux_theta, flux_phi,
                                   m_srcSummaries[srcName].id) );
         accepted = true;
      }
      if (flush || m_events.size() >= m_maxNumEntries) {
         writeEvents();
      }
   }
   if (flush) {
      writeEvents();
   }
   return accepted;
}

void EventContainer::setEventId(const std::string & name, int eventId) {
   typedef std::map<std::string, SourceSummary> id_map_t;
   if (m_srcSummaries.find(name) == m_srcSummaries.end()) {
      m_srcSummaries.insert(
         id_map_t::value_type(name, SourceSummary(eventId)));
   }
}

astro::SkyDir EventContainer::ScZenith(double time) const {
   astro::GPS * gps(astro::GPS::instance());
   gps->time(time);
   return gps->zenithDir();
}

double EventContainer::earthAzimuthAngle(double ra, double dec, 
                                         double time) const {
   astro::SkyDir appDir(ra, dec);
   astro::SkyDir zen_z = ScZenith(time);
   astro::SkyDir tmp = astro::SkyDir(zen_z.ra(), zen_z.dec() - 90.);
   astro::SkyDir zen_x = astro::SkyDir(-tmp());
   astro::SkyDir zen_y = zen_x;
   zen_y().rotate(zen_z(), M_PI/2.);
   double azimuth = (std::atan2(zen_y().dot(appDir()), zen_x().dot(appDir()))
                     *180./M_PI);
   if (azimuth < 0) {
      azimuth += 360.;
   }
   return azimuth;
}

void EventContainer::writeEvents(double obsStopTime) {

   std::string ft1File(outputFileName());
   fitsGen::Ft1File ft1(ft1File, m_events.size(), m_tablename);
   ft1.appendField("MC_SRC_ID", "1J");
   ft1.appendField("MCENERGY", "1E");

   std::vector<Event>::iterator evt = m_events.begin();
   for ( ; ft1.itor() != ft1.end() && evt != m_events.end(); 
         ft1.next(), ++evt) {
      double time = evt->time();
      double ra = evt->appDir().ra();
      double dec = evt->appDir().dec();

      ft1["time"].set(time);
      ft1["energy"].set(evt->energy());
      ft1["ra"].set(ra);
      ft1["dec"].set(dec);
      ft1["l"].set(evt->appDir().l());
      ft1["b"].set(evt->appDir().b());
      ft1["theta"].set(evt->theta());
      ft1["phi"].set(evt->phi());
      ft1["zenith_angle"].set(evt->zenAngle());
      ft1["earth_azimuth_angle"].set(earthAzimuthAngle(ra, dec, time));
      ft1["event_class"].set(evt->eventClass());
      ft1["conversion_type"].set(evt->conversionType());
//       ft1["event_class"].set(evt->conversionType());
//       ft1["conversion_type"].set(evt->conversionType());
      ft1["mc_src_id"].set(evt->eventId());
      ft1["mcenergy"].set(evt->trueEnergy());
   }

// Set stop time to be arrival time of last event if obsStopTime is
// negative (i.e., not set);
   double stop_time(m_events.back().time() - Spectrum::startTime());
   if (obsStopTime > 0) {
      stop_time = obsStopTime;
   }
   ft1.setObsTimes(m_startTime + Spectrum::startTime(),
                   stop_time + Spectrum::startTime());
   
   dataSubselector::Cuts * cuts;
   if (m_cuts) {
      cuts = new dataSubselector::Cuts(*m_cuts);
   } else {
      cuts = new dataSubselector::Cuts();
   }

// Fill the GTI extension with the entire observation in a single GTI.
   dataSubselector::Gti gti;
   gti.insertInterval(m_startTime + Spectrum::startTime(),
                      stop_time + Spectrum::startTime());
   gti.writeExtension(ft1File);

   cuts->addGtiCut(gti);
   cuts->writeDssKeywords(ft1.header());

   ft1.setPhduKeyword("FILENAME", ft1File);
   ft1.setPhduKeyword("VERSION", 1);
   ft1.setPhduKeyword("CREATOR", creator());

   ft1.close();

   cuts->writeGtiExtension(ft1File);
   st_facilities::FitsUtil::writeChecksums(ft1File);
   
// Flush the Event buffer...
   m_events.clear();

// and update the m_fileNum index.
   m_fileNum++;

// Set the start time for next output file to be current stop time.
   m_startTime = stop_time;
}

} // namespace observationSim
