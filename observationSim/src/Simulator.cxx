/**
 * @file Simulator.cxx
 * @brief Implementation for the interface class to FluxSvc::FluxMgr for
 * generating LAT photon events.
 * @author J. Chiang
 * $Header$
 */

#include <string>
#include <iostream>
#include <algorithm>

#include "FluxSvc/../src/EventSource.h"
#include "FluxSvc/../src/CompositeSource.h"
#include "FluxSvc/../src/SpectrumFactoryTable.h"
#include "FluxSvc/../src/FluxMgr.h"
#include "FluxSvc/ISpectrumFactory.h"

#include "latResponse/Irfs.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "LatSc.h"

#include "src/MapSpectrum.h"
#include "FluxSvc/../src/SpectrumFactory.h"
static SpectrumFactory<MapSpectrum> factory;
const ISpectrumFactory& MapSpectrumFactory = factory;

// The following routine seems not to be necessary (for now) since
// CHIMESpectrum, et al. are provided by FluxSvc, and MapSpectrum
// is provided by the above lines.
//
// // "As ever, macros are best avoided."  Stroustrup 1999
// #define DLL_DECL_SPECTRUM(x)   extern const ISpectrumFactory& x##Factory; x##Factory.addRef();

// void fluxLoad() {
// // These are the spectra that we want to make available.
//    DLL_DECL_SPECTRUM( MapSpectrum);
//    DLL_DECL_SPECTRUM( CHIMESpectrum);
//    DLL_DECL_SPECTRUM( AlbedoPSpectrum);
//    DLL_DECL_SPECTRUM( FILESpectrum);
//    DLL_DECL_SPECTRUM( GalElSpectrum);
// //    DLL_DECL_SPECTRUM( SurfaceMuons);
// //    DLL_DECL_SPECTRUM( CrElectron);
// //    DLL_DECL_SPECTRUM( CrProton);
// //    DLL_DECL_SPECTRUM( GRBSpectrum);
// //    DLL_DECL_SPECTRUM( CREMESpectrum);
// }

namespace observationSim {

void Simulator::init(const std::string &sourceName,
                     const std::vector<std::string> &fileList,
                     double totalArea, double startTime) {
   std::vector<std::string> sourceNames;
   sourceNames.push_back(sourceName);
   init(sourceNames, fileList, totalArea, startTime);
}

void Simulator::init(const std::vector<std::string> &sourceNames,
                     const std::vector<std::string> &fileList,
                     double totalArea, double startTime) {
   m_absTime = startTime;
   m_numEvents = 0;
   m_newEvent = 0;
   m_interval = 0;

// This isn't needed (for now).
// // Make the various spectra available.
// //   fluxLoad();

// Create the FluxMgr object, providing access to the sources in the
// various xml files.
   m_fluxMgr = new FluxMgr(fileList);
   m_fluxMgr->setExpansion(1.);    // is this already the default?

// Use the default rocking strategy.
   setRocking();

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
   for (std::vector<std::string>::const_iterator name = sourceNames.begin();
        name != sourceNames.end(); name++) {
      m_source->addSource(m_fluxMgr->source(*name));
   }

// Add a "timetick30s" source to the m_source object, assuming that
// m_source is pointing to a CompositeSource....
   EventSource *clock;
   try {
      clock = m_fluxMgr->source("timetick30s");
   } catch(...) {
      std::cerr << "Simulator::Simulator: \n"
                << "Failed to create a timetick30s source." 
                << std::endl;
      listSources();
      exit(-1);
   }
   try {
      m_source->addSource(clock);
//      dynamic_cast<CompositeSource *>(m_source)->addSource(clock);
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
                           latResponse::Irfs &response, Spacecraft *spacecraft,
                           bool useSimTime) {
   m_useSimTime = useSimTime;
   m_elapsedTime = 0.;

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
            if (events.addEvent(m_newEvent, response, spacecraft)) {
               m_numEvents++;
               if (!m_useSimTime && m_maxNumEvents/20 > 0 &&
                   m_numEvents % (m_maxNumEvents/20) == 0) std::cerr << ".";
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
   if (!m_useSimTime) std::cerr << "!" << std::endl;
}

bool Simulator::done() {
   if (m_useSimTime) {
      return m_elapsedTime >= m_simTime;
   } else {
      return m_numEvents >= m_maxNumEvents;
   }
}   

} // namespace observationSim
