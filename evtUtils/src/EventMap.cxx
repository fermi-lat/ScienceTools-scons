// -*- Mode: c++ -*-


// This Class's header
#include "EvtUtils/EventMap.h"

#include "EvtUtils/EventCategory.h"

#include <TTree.h>

ClassImp(EvtUtils::EventMap);

namespace EvtUtils {

  EventMap::~EventMap() {
    for ( std::map<unsigned,EventCategory*>::iterator itr = m_indexMap.begin(); 
	  itr != m_indexMap.end(); itr++ ) {
      delete itr->second;
      itr->second = 0;
    }  
  }

  EventCategory* EventMap::addCateogry(const std::string& name, unsigned bit,
				       const std::string& shortCut, const std::string& fullCut, 
				       const std::string& comment) {
    // Make sure we aren't overwriting anything
    if ( m_nameMap.count(name) != 0 ) return 0;
    if ( m_indexMap.count(bit) != 0 ) return 0;
    EventCategory* newCat = new EventCategory(name,bit,shortCut,fullCut,comment);
    m_nameMap[name] = newCat;
    m_indexMap[bit] = newCat;
    return newCat;
  }
  
  bool EventMap::initializeShortCuts(TTree& tree) {
    for ( std::map<unsigned,EventCategory*>::iterator itr = m_indexMap.begin(); 
	  itr != m_indexMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      TTreeFormula* test = itr->second->initializeShortCut(tree);
      if ( test == 0 ) return false;
    }  
    return true;
  }

  bool EventMap::initializeFullCuts(TTree& tree) {
    for ( std::map<unsigned,EventCategory*>::iterator itr = m_indexMap.begin(); 
	  itr != m_indexMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      TTreeFormula* test = itr->second->initializeFullCut(tree);
      if ( test == 0 ) return false;
    }  
    return true;  
  }

  bool EventMap::fillShortCutMap(  ) {
    m_shortMap = 0;
    for ( std::map<unsigned,EventCategory*>::iterator itr = m_indexMap.begin(); 
	  itr != m_indexMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      switch ( itr->second->passesShortCut() ) {
      case EventCategory::FailedCut:
      case EventCategory::NoCut:
	break;
      case EventCategory::PassedCut:
	m_shortMap |= ( 1 << itr->first ); 
	break;
      case EventCategory::Error:
      default:
	return false;
      }      
    }  
    return true;  
  }

  bool EventMap::fillFullCutMap(  ) {
    m_fullMap = 0;
    for ( std::map<unsigned,EventCategory*>::iterator itr = m_indexMap.begin(); 
	  itr != m_indexMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      switch ( itr->second->passesFullCut() ) {
      case EventCategory::FailedCut:
      case EventCategory::NoCut:
	break;
      case EventCategory::PassedCut:
	m_fullMap |= ( 1 << itr->first ); 
	break;
      case EventCategory::Error:
      default:
	return false;
      }      
    }  
    return true;  
  }

  bool EventMap::addAliasesToTTree(TTree& tree) {
    for ( std::map<unsigned,EventCategory*>::iterator itr = m_indexMap.begin(); 
	  itr != m_indexMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      itr->second->addAliasesToTTree(tree);
    }
    return true;
  }
  
  EventCategory* EventMap::getCutByName(const std::string& name) {
    std::map<std::string,EventCategory*>::iterator itrFind = m_nameMap.find(name);
    if ( itrFind == m_nameMap.end() ) return 0;
    return itrFind->second;
  }

  EventCategory* EventMap::getCutByIndex(unsigned index) {
    std::map<unsigned,EventCategory*>::iterator itrFind = m_indexMap.find(index);
    if ( itrFind == m_indexMap.end() ) return 0;
    return itrFind->second;
  }

}
