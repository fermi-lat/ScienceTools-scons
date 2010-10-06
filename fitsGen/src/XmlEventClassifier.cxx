/**
 * @file XmlEventClassifier.cxx
 * @brief Wrap evtUtils code to read in xml event class definitions and
 * apply them to a merit file.
 *
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdio>

#include <iostream>
#include <map>
#include <stdexcept>
#include <utility>

#include <TEventList.h>
#include <TFile.h>
#include <TTree.h>

#include "tip/Table.h"

#include "evtUtils/EventClass.h"

#include "fitsGen/XmlEventClassifier.h"

namespace fitsGen {

XmlEventClassifier::XmlEventClassifier(const std::string & xmlFile, 
                                       const std::string & meritFile,
                                       const std::string & filter,
                                       const std::string & evtClassMap)
   : EventClassifier(),
     m_evtClass(evtUtils::EventClass::loadFromXml(xmlFile.c_str())),
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

   UInt_t * photonMap = m_evtClass->getShortMapPtr(evtClassMap);
   if (photonMap == 0) {
      throw std::runtime_error("ShortMapPtr to " + evtClassMap + " not found.");
   }

// Use draw method to get a TEventList
   Long64_t first(0);
   Long64_t nmax(meritTree->GetEntries());
   meritTree->Draw(">>my_event_list", filter.c_str(), "", nmax, first);
   TEventList * eventList = (TEventList *)gDirectory->Get("my_event_list");

   Long64_t nevents = eventList->GetN();
//   std::cout << "number of events: " << nevents << std::endl;
   for (Long64_t j(0); j < nevents; j++) {
      Long64_t ievt = eventList->GetEntry(j);
      meritTree->LoadTree(ievt);
      if (!m_evtClass->fillShortCutMaps()) {
         throw std::runtime_error("error evaluating cuts");
      }
      meritTree->GetEvent(ievt);
      unsigned int run = static_cast<unsigned int>(EvtRun);
      unsigned int event_id = static_cast<unsigned int>(EvtEventId);
      m_bitMaps[std::make_pair(run, event_id)] = *photonMap;
   }
   delete meritTree;
}

XmlEventClassifier::~XmlEventClassifier() throw() {
   delete m_meritFile;
   delete m_evtClass;
}

unsigned int XmlEventClassifier::operator()(unsigned int run, 
                                            unsigned int eventId) const {
   EventClassMap_t::const_iterator it =
      m_bitMaps.find(std::make_pair(run, eventId));
   if (it == m_bitMaps.end()) {
      throw std::runtime_error("invalid run, eventId pair");
   }
   return it->second;
}

unsigned int XmlEventClassifier::
operator()(tip::ConstTableRecord & row) const {
   unsigned int run = static_cast<unsigned int>(row["EvtRun"].get());
   unsigned int eventId = static_cast<unsigned int>(row["EvtEventId"].get());
   return operator()(run, eventId);
}

unsigned int XmlEventClassifier::
operator()(const std::map<std::string, double> & row) const {
   std::map<std::string, double>::const_iterator it = row.find("EvtRun");
   if (it == row.end()) {
      throw std::runtime_error("EvtRun not found in merit tuple.");
   }
   unsigned int run = static_cast<unsigned int>(it->second);

   it = row.find("EvtEventId");
   if (it == row.end()) {
      throw std::runtime_error("EvtEventId not found in merit tuple.");
   }
   unsigned int eventId = static_cast<unsigned int>(it->second);

   return operator()(run, eventId);
}

bool XmlEventClassifier::is_class_member(unsigned int run, 
                                         unsigned int eventId,
                                         unsigned int evtclass) const {
   if (evtclass > 32) {
      throw std::runtime_error("invalid event class identifier");
   }
   unsigned int mask = 1 << evtclass;
   return (operator()(run, eventId) & mask) > 0;
}

std::string XmlEventClassifier::passVersion() const {
   return m_evtClass->getVersion();
}

} // namespace fitsGen
