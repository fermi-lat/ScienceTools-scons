/**
 * @file Simulator.cxx
 * @brief Implementation for the interface class to flux::FluxMgr for
 * generating LAT photon events.
 * @author J. Chiang
 *
 * $Header$
 */

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "facilities/Util.h"

#include "st_facilities/Util.h"

#include "astro/PointingHistory.h"

#include "flux/EventSource.h"
#include "flux/CompositeSource.h"
#include "flux/SpectrumFactoryTable.h"
#include "flux/FluxMgr.h"
#include "flux/ISpectrumFactory.h"

#include "irfInterface/Irfs.h"

#include "flux/SpectrumFactory.h"

#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "observationSim/Simulator.h"
#include "LatSc.h"

namespace observationSim {

astro::GPS::RockType rockTypes[] = {astro::GPS::NONE, 
                                    astro::GPS::UPDOWN,
                                    astro::GPS::SLEWING,
                                    astro::GPS::ONEPERORBIT,
                                    astro::GPS::EXPLICIT,
                                    astro::GPS::POINT,
                                    astro::GPS::HISTORY};
std::vector<astro::GPS::RockType> Simulator::s_rockTypes(rockTypes, 
                                                         rockTypes + 7);

Simulator::~Simulator() {
   delete m_fluxMgr;
   delete m_source;
}

void Simulator::init(const std::string &sourceName,
                     const std::vector<std::string> &fileList,
                     double totalArea, double startTime,
                     const std::string &pointingHistory,
                     double maxSimTime, double pointingHistoryOffset) {
   std::vector<std::string> sourceNames;
   sourceNames.push_back(sourceName);
   init(sourceNames, fileList, totalArea, startTime, pointingHistory,
        maxSimTime, pointingHistoryOffset);
}

void Simulator::init(const std::vector<std::string> &sourceNames,
                     const std::vector<std::string> &fileList,
                     double totalArea, double startTime, 
                     std::string pointingHistory, double maxSimTime,
                     double pointingHistoryOffset) {
   m_absTime = startTime;
   m_numEvents = 0;
   m_newEvent = 0;
   m_interval = 0;

   m_maxSimTime = maxSimTime;

// Create the FluxMgr object, providing access to the sources in the
// various xml files.
   try {
      m_fluxMgr = new FluxMgr(fileList);
   } catch(...) {
      std::ostringstream message;
      message << "\nError reading in the xml model file.\n"
              << "Please check that you are using the correct xml "
              << "format for this tool." << std::endl;
      throw std::runtime_error(message.str());
   }      
   m_fluxMgr->setExpansion(1.);    // is this already the default?

// Set the start of the simulation time in GPS:
   astro::GPS::instance()->time(m_absTime);

   if (pointingHistory != "none" && pointingHistory != "") {
// Use pointing history file.
      facilities::Util::expandEnvVar(&pointingHistory);
      if (st_facilities::Util::fileExists(pointingHistory)) {
         setPointingHistoryFile(pointingHistory, pointingHistoryOffset);
      } else {
         m_formatter->info() << "Pointing history file not found: \n"
                             << pointingHistory << "\n"
                             << "Using default rocking strategy." 
                             << std::endl;
         setRocking();
         pointingHistory = "none";
      }
   } else {
// Use the default rocking strategy.
      setRocking();
   }

// Set the LAT sphere cross-sectional area.
   try {
      EventSource * defaultSource = m_fluxMgr->source("default");
      defaultSource->totalArea(totalArea);
      delete defaultSource;
   } catch(...) {
      m_formatter->info() << "Simulator::Simulator: \n"
                          << "Cannot use default source to set the LAT sphere "
                          << "cross-section.  Leaving it at 6 m^2."
                          << std::endl;
   }

// Create a new pointer to the desired source from m_fluxMgr.
   m_source = new CompositeSource();
   int nsrcs(0);
   for (std::vector<std::string>::const_iterator name = sourceNames.begin();
        name != sourceNames.end(); name++) {
      EventSource * source;
      if ( (source = m_fluxMgr->source(*name)) ) {
         m_source->addSource(source);
         nsrcs++;
         m_formatter->info() << "added source \"" << *name 
                             << "\"" << std::endl;
      } else {
         m_formatter->info() << "Simulator::init: \n"
                             << "FluxMgr failed to find a source named \""
                             << *name << "\"" << std::endl;
      }
   }
   if (nsrcs == 0) {
      m_formatter->err() << "Simulator::init: \n"
                         << "FluxMgr has failed to add any valid "
                         << "photon sources to the model."
                         << std::endl;
      std::exit(1);
   }

   if (pointingHistory == "none" || pointingHistory == "") {
// Add a "timetick30s" source to the m_source object.
      EventSource *clock;
      try {
         clock = m_fluxMgr->source("obsSim_timetick30s");
      } catch(...) {
         m_formatter->err() << "Simulator::Simulator: \n"
                            << "Failed to create a obsSim_timetick30s source." 
                            << std::endl;
         listSources();
         std::exit(1);
      }
      try {
         m_source->addSource(clock);
      } catch(...) {
         m_formatter->err() << "Failed to add a timetick30s source to the "
                            << "CompositeSource object."
                            << std::endl;
         listSources();
         std::exit(1);
      }
   }
}

void Simulator::listSources() const {
   m_formatter->info() << "List of available sources:" << std::endl;
   std::list<std::string> source_list = m_fluxMgr->sourceList();
   
   for (std::list<std::string>::const_iterator it = source_list.begin()
           ; it != source_list.end(); it++) {
      m_formatter->info() << '\t' << *it << std::endl;
   }
}

void Simulator::listSpectra() const {
   m_formatter->info() << "List of loaded Spectrum objects: " << std::endl;
   std::list<std::string> 
      spectra(SpectrumFactoryTable::instance()->spectrumList());
   for( std::list<std::string>::const_iterator it = spectra.begin(); 
        it != spectra.end(); ++it) { 
      m_formatter->info() << '\t'<< *it << std::endl;
   }
}

void Simulator::makeEvents(EventContainer &events, 
                           ScDataContainer &scData, 
                           std::vector<irfInterface::Irfs *> &respPtrs, 
                           Spacecraft *spacecraft,
                           bool useSimTime) {
   m_useSimTime = useSimTime;
   m_elapsedTime = 0.;

// Loop over event generation steps until done.
   while (!done()) {

// Check if we need a new event from m_source.
      if (m_newEvent == 0) {
// Unfortunately, we need to check for the
// astro::PointingHistory::TimeRangeError in case we are using a
// pointing history file since steady sources can still request a
// time beyond the end time of that file.
         try {
            m_newEvent = m_source->event(m_absTime);
            if (!m_newEvent->enabled()) { 
// There are no more events from any sources (allegedly), so we
// exit the loop.
               break;
            }
            m_newEvent->code(m_source->numSource());
            m_interval = m_source->interval(m_absTime);
         } catch (astro::PointingHistory::TimeRangeError & eObj) {
            m_formatter->info(4) << "Caught TimeRangeError: " 
                                 << eObj.what() << "\n"
                                 << "Exiting." << std::endl;
            break;
         }
      }

// Process m_newEvent either if we are accumulating counts or if the
// event arrives within the present observing window given by
// m_simTime.
      if ( !m_useSimTime ||  (m_elapsedTime+m_interval < m_simTime) ) {
         m_absTime += m_interval;
         m_elapsedTime += m_interval;
         m_fluxMgr->pass(m_interval);
         
         std::string name = m_newEvent->fullTitle();
         if (name.find("TimeTick") != std::string::npos) {
            if (!m_usePointingHistory) {
               scData.addScData(m_newEvent, spacecraft);
            }
         } else {
            if (events.addEvent(m_newEvent, respPtrs, spacecraft)) {
               m_numEvents++;
            }
         }
// EventSource::event(...) does not generate a pointer to a new object
// (as of 07/02/03), so there's no need to delete m_newEvent.
         m_newEvent = 0;
      } else if (m_useSimTime) {
// No more events to process for this observation window, so advance
// to the end of the window, updating all of the time accumulators.
         m_absTime += (m_simTime - m_elapsedTime);
         m_fluxMgr->pass(m_simTime - m_elapsedTime);
         m_elapsedTime = m_simTime;
      }
   } // while (!done())
}

bool Simulator::done() {
   if (m_elapsedTime > m_maxSimTime) {
      return true;
   }
   if (m_useSimTime) {
      return m_elapsedTime >= m_simTime;
   } else {
      return m_numEvents >= m_maxNumEvents;
   }
}   

} // namespace observationSim
