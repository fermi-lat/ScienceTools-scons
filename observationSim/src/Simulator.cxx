/**
 * @file Simulator.cxx
 * @brief Implementation for the interface class to flux::FluxMgr for
 * generating LAT photon events.
 * @author J. Chiang
 *
 * $Header$
 */

#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "CLHEP/Geometry/Vector3D.h"

#include "facilities/Util.h"

#include "st_facilities/Util.h"

#include "flux/EventSource.h"
#include "flux/CompositeSource.h"
#include "flux/SpectrumFactoryTable.h"
#include "flux/FluxMgr.h"
#include "flux/ISpectrumFactory.h"

#include "irfInterface/Irfs.h"

#include "flux/SpectrumFactory.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "LatSc.h"
#include "Verbosity.h"

namespace observationSim {

Simulator::~Simulator() {
   delete m_fluxMgr;
   delete m_source;
}

void Simulator::init(const std::string &sourceName,
                     const std::vector<std::string> &fileList,
                     double totalArea, double startTime,
                     const std::string &pointingHistory,
                     double maxSimTime) {
   std::vector<std::string> sourceNames;
   sourceNames.push_back(sourceName);
   init(sourceNames, fileList, totalArea, startTime, pointingHistory,
        maxSimTime);
}

void Simulator::init(const std::vector<std::string> &sourceNames,
                     const std::vector<std::string> &fileList,
                     double totalArea, double startTime, 
                     std::string pointingHistory, double maxSimTime) {
   m_absTime = startTime;
   m_numEvents = 0;
   m_newEvent = 0;
   m_interval = 0;

   m_maxSimTime = maxSimTime;

// Create the FluxMgr object, providing access to the sources in the
// various xml files.
   m_fluxMgr = new FluxMgr(fileList);
   m_fluxMgr->setExpansion(1.);    // is this already the default?

   if (pointingHistory != "none" && pointingHistory != "") {
// Use pointing history file.
      facilities::Util::expandEnvVar(&pointingHistory);
      if (st_facilities::Util::fileExists(pointingHistory)) {
         setRocking(5, 0);
         setPointingHistoryFile(pointingHistory);
      } else {
         if (print_output()) {
            std::cout << "Pointing history file not found: \n"
                      << pointingHistory << "\n"
                      << "Using default rocking strategy." << std::endl;
         }
         setRocking();
      }
   } else {
// Use the default rocking strategy.
      setRocking();
   }

// Set the LAT sphere cross-sectional area.
   try {
      EventSource *defaultSource = m_fluxMgr->source("default");
      defaultSource->totalArea(totalArea);
      delete defaultSource;
   } catch(...) {
      std::cerr << "Simulator::Simulator: \n"
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
         if (print_output()) {
            std::cout << "added source \"" << *name << "\"" << std::endl;
         }
      } else {
         if (print_output()) {
            std::cout << "Simulator::init: \n"
                      << "FluxMgr failed to find a source named \""
                      << *name << "\"" << std::endl;
         }
      }
   }
   if (nsrcs == 0) {
      if (print_output()) {
         std::cout << "Simulator::init: \n"
                   << "FluxMgr has failed to add any valid "
                   << "photon sources to the model."
                   << std::endl;
      }
      exit(-1);
   }

// Add a "timetick30s" source to the m_source object.
   EventSource *clock;
   try {
      clock = m_fluxMgr->source("obsSim_timetick30s");
   } catch(...) {
      std::cerr << "Simulator::Simulator: \n"
                << "Failed to create a obsSim_timetick30s source." 
                << std::endl;
      listSources();
      exit(-1);
   }
   try {
      m_source->addSource(clock);
   } catch(...) {
      std::cerr << "Failed to add a timetick30s source to the "
                << "CompositeSource object."
                << std::endl;
      listSources();
      exit(-1);
   }
}

void Simulator::listSources() const {
   std::cerr << "List of available sources:" << std::endl;
   std::list<std::string> source_list = m_fluxMgr->sourceList();

   for (std::list<std::string>::const_iterator it = source_list.begin()
           ; it != source_list.end(); it++) {
      std::cerr << '\t' << *it << std::endl;
   }
}

void Simulator::listSpectra() const {
   std::cerr << "List of loaded Spectrum objects: " << std::endl;
   std::list<std::string> 
      spectra(SpectrumFactoryTable::instance()->spectrumList());
   for( std::list<std::string>::const_iterator it = spectra.begin(); 
        it != spectra.end(); ++it) { 
      std::cerr << '\t'<< *it << std::endl;
   }
}

void Simulator::makeEvents(EventContainer &events, ScDataContainer &scData, 
                           irfInterface::Irfs &response,
                           Spacecraft *spacecraft,
                           bool useSimTime, EventContainer *allEvents) {
   std::vector<irfInterface::Irfs *> respPtrs;
   respPtrs.push_back(&response);
   makeEvents(events, scData, respPtrs, spacecraft, useSimTime, 
              allEvents);
}

void Simulator::makeEvents(EventContainer &events, 
                           ScDataContainer &scData, 
                           std::vector<irfInterface::Irfs *> &respPtrs, 
                           Spacecraft *spacecraft,
                           bool useSimTime, 
                           EventContainer *allEvents) {
   m_useSimTime = useSimTime;
   m_elapsedTime = 0.;

// Insert the very first entry in the scData file.
   scData.addScData(m_absTime, spacecraft);

// Enclose loop in outer try block, catching a GPS-thrown exception when
// time exceeds pointing history database.
   try {
 // Loop over event generation steps until done.
   while (!done()) {

// Check if we need a new event from m_source.
      if (m_newEvent == 0) {
         m_newEvent = m_source->event(m_absTime);
         m_interval = m_source->interval(m_absTime);
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
            scData.addScData(m_newEvent, spacecraft);
         } else {
            if (allEvents != 0) {
               allEvents->addEvent(m_newEvent, respPtrs, spacecraft, 
                                   false, true);
            }
            if (events.addEvent(m_newEvent, respPtrs, spacecraft)) {
               m_numEvents++;
            }
         }
// EventSource::event(...) does not generate a pointer to a new object
// (as of 07/02/03), so there's no need to delete m_newEvent.
         m_newEvent = 0;

      } else if (m_useSimTime) {
// No more events to process for this observation window, so advance
// to the end of the window, updating all of the time accumlators.
         m_absTime += (m_simTime - m_elapsedTime);
         m_fluxMgr->pass(m_simTime - m_elapsedTime);
         m_elapsedTime = m_simTime;
      }
   } // while (!done())

   } catch (std::exception & eObj) {
      if (!st_facilities::Util::expectedException(eObj,"Time out of Range!")) {
         throw;
      }
   }
//    if (!m_useSimTime) std::cerr << "!" << std::endl;
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
