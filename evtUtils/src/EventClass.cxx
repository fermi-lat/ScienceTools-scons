// -*- Mode: c++ -*-


// This Class's header
#include "EvtUtils/EventClass.h"

#include "EvtUtils/EventMap.h"

#include <TTree.h>

//#define USE_ROOT_XML 1

#ifndef USE_ROOT_XML
#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"
#include "xercesc/dom/DOMElement.hpp"
#else
#include <TXMLEngine.h>
#endif

ClassImp(EvtUtils::EventClass);

namespace EvtUtils {

  EventClass::~EventClass() {
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      delete itr->second;
      itr->second = 0;
    }  
  }
  
  EventMap* EventClass::addEventMap(const std::string& mapName, const std::string& altName) {
    if ( m_evtMap.count(mapName) != 0 ) {
      return 0;
    }
    EventMap* evtMap = new EventMap(mapName,altName);
    m_evtMap[mapName] = evtMap;
    return evtMap;
  }
  
  bool EventClass::addAliasesToTTree(TTree& tree) {
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      if ( ! itr->second->addAliasesToTTree(tree) ) return false;
    }
    return true;
    
  }
  
  bool EventClass::addShortBranchesToTTree(TTree& tree) {  
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      std::string leafName = itr->first;
      leafName += "/i";
      void* ptr = itr->second->getShortMapPtr();
      TBranch* b = tree.Branch(itr->first.c_str(),ptr,leafName.c_str());
      if ( b == 0 ) return false;
    }
    return true;  
  }

  bool EventClass::addFullBranchesToTTree(TTree& tree) {  
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      std::string branchName = itr->first + "_Full";
      std::string leafName = branchName + "/i";
      void* ptr = itr->second->getFullMapPtr();
      TBranch* b = tree.Branch(branchName.c_str(),ptr,leafName.c_str());
      if ( b == 0 ) return false;
    }
    return true;  
  }
  
  bool EventClass::initializeShortCuts(TTree& tree) {
    if ( &tree == m_cachedTree ) return true;
    m_cachedTree = 0;
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      if ( ! itr->second->initializeShortCuts(tree) ) return false;
    }
    m_cachedTree = &tree;  
    return true;  
  }
  
  bool EventClass::initializeFullCuts(TTree& tree){
    if ( &tree == m_cachedTree ) return true;
    m_cachedTree = 0;
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      if ( ! itr->second->initializeFullCuts(tree) ) return false;
    }
    m_cachedTree = &tree;  
    return true;  
  }
  
  bool EventClass::fillShortCutMaps( ){
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      if ( ! itr->second->fillShortCutMap() ) return false;
    }
    return true;  
  }
  
  bool EventClass::fillFullCutMaps( ){
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      if ( ! itr->second->fillFullCutMap() ) return false;
    }
    return true;    
  }
  
  EventMap* EventClass::getEventMapByName(const std::string& name) {
    std::map<std::string,EventMap*>::iterator itrFind = m_evtMap.find(name);
    if ( itrFind == m_evtMap.end() ) return 0;
    return itrFind->second;
  }

  void EventClass::getEvtMapNames(std::list<std::string>& names) {
    names.clear();
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      names.push_back(itr->first);
    }
  }
  
  UInt_t* EventClass::getShortMapPtr(const std::string& name) {
    std::map<std::string,EventMap*>::iterator itrFind = m_evtMap.find(name);
    if ( itrFind == m_evtMap.end() ) return 0;
    return itrFind->second->getShortMapPtr();
  }

  UInt_t* EventClass::getFullMapPtr(const std::string& name) {
    std::map<std::string,EventMap*>::iterator itrFind = m_evtMap.find(name);
    if ( itrFind == m_evtMap.end() ) return 0;
    return itrFind->second->getFullMapPtr();
  }

#ifndef USE_ROOT_XML

  EventClass* EventClass::loadFromXml(const std::string& fileName) {
    
    static const std::string EventClass("EventClass");
    static const std::string EventMap("EventMap");  
    static const std::string EventCategory("EventCategory");
    static const std::string ShortCut("ShortCut");
    static const std::string FullCut("FullCut");
    static const std::string Comment("Comment");
    
    using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;
    using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
    using XERCES_CPP_NAMESPACE_QUALIFIER DOMNode;
    
    xmlBase::XmlParser parser;

    DOMDocument* doc = parser.parse(fileName.c_str());
    if ( doc == 0 ) return 0;
    DOMElement* top = doc->getDocumentElement();					       
    if ( ! xmlBase::Dom::checkTagName(top,EventClass) ) {
      delete doc;
      return 0;
    }
    
    std::string version = xmlBase::Dom::getAttribute(top,"version");
    if ( version.empty() ) {
      delete doc;
      return 0;    
    }
    
    EvtUtils::EventClass* evtClass = new EvtUtils::EventClass(version);
  
    std::vector<DOMElement*> eventMaps;
    xmlBase::Dom::getChildrenByTagName(top,EventMap,eventMaps);
    
    for ( std::vector<DOMElement*>::iterator itrMap = eventMaps.begin(); itrMap != eventMaps.end(); itrMap++ ) {
      DOMElement* elemMap = *itrMap;
      std::string mapName = xmlBase::Dom::getAttribute(elemMap,"mapName");
      std::string altName = xmlBase::Dom::getAttribute(elemMap,"altName");
      EvtUtils::EventMap* evtMap = evtClass->addEventMap(mapName,altName);
      if ( evtMap == 0 ) {
	delete doc;
	delete evtClass;
	return 0;        
      }
      std::vector<DOMElement*> eventCats;
      xmlBase::Dom::getChildrenByTagName(elemMap,EventCategory,eventCats);    
      for ( std::vector<DOMElement*>::iterator itrCat = eventCats.begin(); itrCat != eventCats.end(); itrCat++ ) {
	DOMElement* elemCat = *itrCat;
	std::string catName = xmlBase::Dom::getAttribute(elemCat,"name");
	int bitVal = xmlBase::Dom::getIntAttribute(elemCat,"bit");
	if ( bitVal < 0 || bitVal > 31 ) {
	  delete doc;
	  delete evtClass;
	  return 0;        	
	}
	DOMElement* elemShortCut = xmlBase::Dom::findFirstChildByName(elemCat,ShortCut);
	if ( elemShortCut == 0 ) {
	  delete doc;
	  delete evtClass;
	  return 0;        		
	}
	std::string shortCut = xmlBase::Dom::getTextContent(elemShortCut);      
	std::string fullCut;
	std::string comment;
	DOMElement* elemFullCut = xmlBase::Dom::findFirstChildByName(elemCat,FullCut); 
	DOMElement* elemComment = xmlBase::Dom::findFirstChildByName(elemCat,Comment); 
	if ( elemFullCut ) {
	  fullCut = xmlBase::Dom::getTextContent(elemFullCut); 
	}
	if ( elemComment ) {
	  comment = xmlBase::Dom::getTextContent(elemComment); 
	}
	EvtUtils::EventCategory* evtCat = evtMap->addCateogry(catName,bitVal,shortCut,fullCut,comment);
	if ( evtCat == 0 ) {
	  delete doc;
	  delete evtClass;
	  return 0;        			
	}
      }    
    }
    return evtClass;
  }

#else

  EventClass* EventClass::loadFromXml(const std::string& fileName) {
    
    static const std::string EventClass("EventClass");
    static const std::string EventMap("EventMap");  
    static const std::string EventCategory("EventCategory");
    static const std::string ShortCut("ShortCut");
    static const std::string FullCut("FullCut");
    static const std::string Comment("Comment");

    TXMLEngine theEngine;
    
    XMLDocPointer_t doc = theEngine.ParseFile(fileName.c_str());
    if ( doc == 0 ) {
      return 0;
    }
    
    XMLNodePointer_t top = theEngine.DocGetRootElement(doc);
    if ( EventClass != theEngine.GetNodeName(top)  ) {
      return 0;
    }
    
    const char* version = theEngine.GetAttr(top,"version");
    if ( version ==0 ) {
      return 0;    
    }
    
    EvtUtils::EventClass* evtClass = new EvtUtils::EventClass(version);
  
    XMLNodePointer_t elemMap = theEngine.GetChild(top);
    while ( elemMap != 0 ) {
      const char* mapName = theEngine.GetAttr(elemMap,"mapName");
      const char* altName = theEngine.GetAttr(elemMap,"altName");
      EvtUtils::EventMap* evtMap = evtClass->addEventMap(mapName,altName);
      if ( evtMap == 0 ) {
	delete evtClass;
	return 0;        
      }
      
      XMLNodePointer_t elemCat = theEngine.GetChild(elemMap);
      while ( elemCat != 0) {
	const char* catName = theEngine.GetAttr(elemCat,"name");
	Int_t bitVal = theEngine.GetIntAttr(elemCat,"bit");
	if ( bitVal < 0 || bitVal > 31 ) {
	  delete evtClass;
	  return 0;        	
	}
	XMLNodePointer_t child = theEngine.GetChild(elemCat);
	std::string shortCut;
	std::string fullCut;
	std::string comment;
	while ( child != 0 ) {
	  std::string childName = theEngine.GetNodeName(child);
	  if ( childName == ShortCut ){
	    shortCut = theEngine.GetNodeContent(child);
	  } else if ( childName == FullCut ) {
	    fullCut = theEngine.GetNodeContent(child);
	  } else if ( childName == Comment ) {
	    comment = theEngine.GetNodeContent(child);
	  }
	  child = theEngine.GetNext(child);	  
	}
	EvtUtils::EventCategory* evtCat = evtMap->addCateogry(catName,bitVal,shortCut,fullCut,comment);
	if ( evtCat == 0 ) {
	  delete evtClass;
	  return 0;        			
	}
	elemCat = theEngine.GetNext(elemCat);
      }
      elemMap = theEngine.GetNext(elemMap);      
    }
    return evtClass;
  }

#endif

}
