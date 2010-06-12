/**
 * @file XmlEventClassifier.cxx
 * @brief Wrap EvtUtils code to read in xml event class definitions and
 * apply them to a merit file.
 *
 * $Header$
 */

#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>

#include <TFile.h>
#include <TTree.h>

#include "EvtUtils/EventClass.h"

#include "fitsGen/XmlEventClassifier.h"

namespace fitsGen {

XmlEventClassifier::XmlEventClassifier(const std::string & xmlFile, 
                                       const std::string & meritFile) 
   : m_evtClass(EvtUtils::EventClass::loadFromXml(xmlFile.c_str())),
     m_meritFile(TFile::Open(meritFile.c_str())) {

   if (m_evtClass == 0) {
      throw std::runtime_error("Failed to load " + xmlFile);
   }

   if (m_meritFile == 0) {
      throw std::runtime_error("Failed to load " + meritFile);
   }

   TTree * meritTree = dynamic_cast<TTree *>(m_meritFile->Get("MeritTuple"));

   if (!m_evtClass->initializeShortCuts(*meritTree)) {
      throw std::runtime_error("error initializing cuts");
   };

   UInt_t EvtRun;
   UInt_t EvtEventId;

   meritTree->SetBranchAddress("EvtRun", &EvtRun);
   meritTree->SetBranchAddress("EvtEventId", &EvtEventId);

   UInt_t * photonMap = m_evtClass->getShortMapPtr("FT1EventClass");
   if (photonMap == 0) {
      throw std::runtime_error("ShortMapPtr to FT1EventClass not found.");
   }

   Long64_t nevents = meritTree->GetEntries();
   std::cout << "number of events: " << nevents << std::endl;
   for (Long64_t ievt(0); ievt < nevents; ievt++) {
      meritTree->LoadTree(ievt);
      if (!m_evtClass->fillShortCutMaps()) {
         throw std::runtime_error("error evaluating cuts");
      };
      meritTree->GetEvent(ievt);
      unsigned int run = static_cast<unsigned int>(EvtRun);
      unsigned int event_id = static_cast<unsigned int>(EvtEventId);
      m_eventClasses[std::make_pair(run, event_id)] = *photonMap;
   }
}

unsigned int XmlEventClassifier::operator()(unsigned int run, 
                                            unsigned int eventId) const {
   EventClassMap_t::const_iterator it =
      m_eventClasses.find(std::make_pair(run, eventId));
   if (it == m_eventClasses.end()) {
      throw std::runtime_error("invalid run, eventId pair");
   }
   return it->second;
}

} // namespace fitsGen
