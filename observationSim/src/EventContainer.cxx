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

#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/JamesRandom.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"
#include "tip/Header.h"

#include "astro/SkyDir.h"
#include "astro/GPS.h"

#include "flux/EventSource.h"

#include "st_facilities/FitsUtil.h"

#include "irfInterface/Irfs.h"

#include "dataSubselector/Cuts.h"
#include "dataSubselector/Gti.h"

#include "observationSim/EventContainer.h"
#include "observationSim/Spacecraft.h"
#include "Verbosity.h"

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
                                  astro::SkyDir &xAxis) {
   
// Build a vector of effective area accumulated over the vector
// of response object pointers.
//
// First, fill a vector with the individual values.
      std::vector<double> effAreas(respPtrs.size());
      std::vector<double>::iterator eaIt = effAreas.begin();
      std::vector<irfInterface::Irfs *>::iterator respIt = respPtrs.begin();
      while (eaIt != effAreas.end() && respIt != respPtrs.end()) {
         *eaIt = (*respIt)->aeff()->value(energy, sourceDir, zAxis, xAxis);
         eaIt++;
         respIt++;
      }

// Compute the cumulative distribution.
      std::partial_sum(effAreas.begin(), effAreas.end(), effAreas.begin());

// The total effective area.
      double effAreaTot = *(effAreas.end() - 1);

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
   
   char * root_path = std::getenv("OBSERVATIONSIMROOT");
   if (root_path != 0) {
      m_ftTemplate = std::string(root_path) + "/data/ft1.tpl";
   } else {
      throw std::runtime_error(std::string("Environment variable ")
                               + "OBSERVATIONSIMROOT not set.");
   }
}

bool EventContainer::addEvent(EventSource *event, 
                              std::vector<irfInterface::Irfs *> &respPtrs, 
                              Spacecraft *spacecraft,
                              bool flush, bool alwaysAccept) {
   
   std::string particleType = event->particleName();
   double time = event->time();
   double energy = event->energy();
   Hep3Vector launchDir = event->launchDir();

   double arg = launchDir.z();
   double flux_theta = ::my_acos(arg);
   double flux_phi = atan2(launchDir.y(), launchDir.x());

   HepRotation rotMatrix = spacecraft->InstrumentToCelestial(time);
   astro::SkyDir sourceDir(rotMatrix(-launchDir), astro::SkyDir::EQUATORIAL);

   astro::SkyDir zAxis = spacecraft->zAxis(time);
   astro::SkyDir xAxis = spacecraft->xAxis(time);

   std::string srcName(event->name());
   setEventId(srcName);

   if (alwaysAccept) {
      m_events.push_back( Event(time, energy, 
                                sourceDir, sourceDir, zAxis, xAxis,
                                ScZenith(time), 0, energy, flux_theta,
                                flux_phi, m_eventIds[srcName]) );
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
                                    energy, sourceDir, zAxis, xAxis))
        && !spacecraft->inSaa(time) 
        && RandFlat::shoot() < spacecraft->livetimeFrac(time) ) {

      astro::SkyDir appDir 
         = respPtr->psf()->appDir(energy, sourceDir, zAxis, xAxis);
      double appEnergy 
         = respPtr->edisp()->appEnergy(energy, sourceDir, zAxis, xAxis);

      std::map<std::string, double> evtParams;
      evtParams["ENERGY"] = appEnergy;
      evtParams["RA"] = appDir.ra();
      evtParams["DEC"] = appDir.dec();
      if (m_cuts == 0 || m_cuts->accept(evtParams)) {
         m_events.push_back( Event(time, appEnergy, 
                                   appDir, sourceDir, zAxis, xAxis,
                                   ScZenith(time), respPtr->irfID(), 
                                   energy, flux_theta, flux_phi,
                                   m_eventIds[srcName]) );
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

void EventContainer::setEventId(const std::string & name) {
   typedef std::map<std::string, int> id_map_t;
   if (m_eventIds.find(name) == m_eventIds.end()) {
      m_eventIds.insert(id_map_t::value_type(name, m_eventIds.size()));
   }
}

astro::SkyDir EventContainer::ScZenith(double time) const {
   astro::GPS * gps = astro::GPS::instance();
   gps->getPointingCharacteristics(time);
   double lon_zenith = gps->RAZenith()*M_PI/180.;
   double lat_zenith = gps->DECZenith()*M_PI/180.;
   return astro::SkyDir(Hep3Vector(cos(lat_zenith)*cos(lon_zenith),
                                   cos(lat_zenith)*sin(lon_zenith),
                                   sin(lat_zenith)),
                        astro::SkyDir::EQUATORIAL);
}

double EventContainer::earthAzimuthAngle(double ra, double dec, 
                                         double time) const {
   astro::SkyDir appDir(ra, dec);
   astro::SkyDir zen_z = ScZenith(time);
   astro::SkyDir tmp = astro::SkyDir(zen_z.ra(), zen_z.dec() - 90.);
   astro::SkyDir zen_x = astro::SkyDir(-tmp());
   astro::SkyDir zen_y = zen_x;
   zen_y().rotate(zen_z(), M_PI/2.);
   double azimuth = std::atan2(zen_y().dot(appDir()), zen_x().dot(appDir()));
   return azimuth*180./M_PI;
}

void EventContainer::writeEvents(double obsStopTime) {

   std::string ft1File = outputFileName();
   tip::IFileSvc::instance().createFile(ft1File, m_ftTemplate);
   tip::Table * my_table = 
      tip::IFileSvc::instance().editTable(ft1File, m_tablename);
   my_table->appendField("MC_SRC_ID", "1I");
   my_table->setNumRecords(m_events.size());
   tip::Table::Iterator it = my_table->begin();
   tip::Table::Record & row = *it;
   std::vector<Event>::iterator evt = m_events.begin();
   for ( ; it != my_table->end(), evt != m_events.end(); ++it, ++evt) {
      double time = evt->time();
      double ra = evt->appDir().ra();
      double dec = evt->appDir().dec();

      row["time"].set(time);
      row["energy"].set(evt->energy());
      row["ra"].set(ra);
      row["dec"].set(dec);
      row["theta"].set(evt->theta());
      row["phi"].set(evt->phi());
      row["zenith_angle"].set(evt->zenAngle());
      row["earth_azimuth_angle"].set(earthAzimuthAngle(ra, dec, time));
      try {
         row["conversion_layer"].set(evt->convLayer());
      } catch (std::exception &eObj) {
         if (print_output()) {
            std::cout << eObj.what() << std::endl;
         }
         std::exit(-1);
      }
      tip::Table::Vector<short> calibVersion = row["calib_version"];
      for (int i = 0; i < 3; i++) {
         calibVersion[i] = 1;
      }
      row["mc_src_id"].set(evt->eventId());
   }
//    double stop_time;
//    if (m_stopTime <= m_startTime) {
//       it = my_table->end();
//       --it;
//       row["time"].get(stop_time);
//    } else {
//       stop_time = m_stopTime;
//    }

// Set stop time to be arrival time of last event if obsStopTime is
// negative (i.e., not set);
   double stop_time(m_events.back().time());
   if (obsStopTime > 0) {
      stop_time = obsStopTime;
   }

   writeDateKeywords(my_table, m_startTime, stop_time);

// Fill the GTI extension, with the entire observation in a single GTI.
   dataSubselector::Gti gti;
   gti.insertInterval(m_startTime, stop_time);
   gti.writeExtension(ft1File);

   tip::Table * gti_table = 
      tip::IFileSvc::instance().editTable(ft1File, "GTI");
   writeDateKeywords(gti_table, m_startTime, stop_time);

   dataSubselector::Cuts * cuts;
   if (m_cuts) {
      cuts = new dataSubselector::Cuts(*m_cuts);
   } else {
      cuts = new dataSubselector::Cuts();
   }
   cuts->addGtiCut(*gti_table);
   cuts->writeDssKeywords(my_table->getHeader());

   delete cuts;
   delete my_table;
   delete gti_table;

// Take care of date keywords in primary header.
   tip::Image * phdu = tip::IFileSvc::instance().editImage(ft1File, "");
   writeDateKeywords(phdu, m_startTime, stop_time, false);
   delete phdu;

   st_facilities::FitsUtil::writeChecksums(ft1File);
   
// Flush the Event buffer...
   m_events.clear();

// and update the m_fileNum index.
   m_fileNum++;

// Set the start time for next output file to be current stop time.
   m_startTime = stop_time;
}

} // namespace observationSim
