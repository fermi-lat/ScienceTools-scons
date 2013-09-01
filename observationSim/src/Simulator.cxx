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

#include "Likelihood/Aeff.h"
#include "Likelihood/Psf.h"
#include "Likelihood/LikelihoodException.h"

#include "FluxSvc/../src/EventSource.h"
#include "FluxSvc/../src/CompositeSource.h"
#include "FluxSvc/../src/SpectrumFactoryTable.h"
#include "FluxSvc/../src/FluxMgr.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"

namespace observationSim {

Simulator::Simulator(const std::string &sourceName,
                     const std::vector<std::string> &fileList,
                     double totalArea, double startTime) {

   m_absTime = startTime;
   m_numEvents = 0;
   m_elapsedTime = 0.;

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
   m_source = m_fluxMgr->source(sourceName);

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
      dynamic_cast<CompositeSource *>(m_source)->addSource(clock);
   } catch(...) {
      std::cerr << "Failed to add a timetick30s source to "
                << sourceName << ".  Is it a CompsiteSource?"
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

void Simulator::readResponseData(const std::string &caldbPath, int hdu) {

   Likelihood::Psf *psf = Likelihood::Psf::instance();
   std::string psf_file = caldbPath + "/psf_lat.fits";
   psf->readPsfData(psf_file, hdu);

   Likelihood::Aeff *aeff = Likelihood::Aeff::instance();
   std::string aeff_file = caldbPath + "/aeff_lat.fits";
   aeff->readAeffData(aeff_file, hdu);
}

void Simulator::makeEvents(EventContainer &events, ScDataContainer &scData, 
                           bool useSimTime) {
   m_useSimTime = useSimTime;

// Loop over event generation steps until done.
   while (!done()) {
      EventSource *f = m_source->event(m_absTime);
      double interval = m_source->interval(m_absTime);
      m_absTime += interval;
      m_elapsedTime += interval;
      m_fluxMgr->pass(interval);
      scData.addScData(f);
      if (events.addEvent(f, *m_fluxMgr)) {
         m_numEvents++;
         if (m_maxNumEvents/20 > 0 &&
             m_numEvents % (m_maxNumEvents/20) == 0) std::cerr << ".";
      }
   }
   std::cerr << "!" << std::endl;
}

// "As ever, macros are best avoided."  Stroustrup 1999
#define DLL_DECL_SPECTRUM(x)   extern const ISpectrumFactory& x##Factory; x##Factory.addRef();

void Simulator::fluxLoad() {
   // These are the spectra that we want to make available.
//    DLL_DECL_SPECTRUM( CHIMESpectrum);
//    DLL_DECL_SPECTRUM( AlbedoPSpectrum);
//    DLL_DECL_SPECTRUM( FILESpectrum);
//    DLL_DECL_SPECTRUM( GalElSpectrum);
//    DLL_DECL_SPECTRUM( SurfaceMuons);
   //  DLL_DECL_SPECTRUM( CrElectron);
   //  DLL_DECL_SPECTRUM( CrProton);
   //  DLL_DECL_SPECTRUM( GRBSpectrum);
   //  DLL_DECL_SPECTRUM( CREMESpectrum);
}

bool Simulator::done() {
   if (m_useSimTime) {
      return m_elapsedTime >= m_simTime;
   } else {
      return m_numEvents >= m_maxNumEvents;
   }
}   

} //observationSim
