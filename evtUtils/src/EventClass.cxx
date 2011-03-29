// -*- Mode: c++ -*-


// This Class's header
#include "evtUtils/EventClass.h"

#include "evtUtils/EventMap.h"
#include "evtUtils/EventCategory.h"
#include "evtUtils/AliasDict.h"

#include <TTree.h>
#include <iostream>

//#define USE_ROOT_XML 1

#ifndef USE_ROOT_XML
#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"
#include "xercesc/dom/DOMElement.hpp"
#else
#include <TXMLEngine.h>
#endif

//ClassImp(evtUtils::EventClass);

namespace evtUtils {

  EventClass::~EventClass() {
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      delete itr->second;
      itr->second = 0;
    }  
    delete m_aliasDict;
  }
  
  EventMap* EventClass::addEventMap(const std::string& mapName, const std::string& altName) {
    if ( m_evtMap.count(mapName) != 0 ) {
      return 0;
    }
    EventMap* evtMap = new EventMap(mapName,altName);
    m_evtMap[mapName] = evtMap;
    return evtMap;
  }
  
  void EventClass::addAlias(const std::string& aliasName, const std::string& aliasVal) {
    if ( m_aliasDict == 0 ) {
      m_aliasDict = new AliasDict();
    }
    m_aliasDict->addAlias(aliasName,aliasVal);
  }

  bool EventClass::addAliasesToTTree(TTree& tree) {
    if ( m_aliasDict ){
      m_aliasDict->addAliasesToTTree(tree);
    }
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
    addAliasesToTTree(tree);
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
    addAliasesToTTree(tree);
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      if ( ! itr->second->initializeFullCuts(tree) ) return false;
    }
    m_cachedTree = &tree;  
    return true;  
  }
  
  bool EventClass::initializeBoth(TTree& tree){
    if ( &tree == m_cachedTree ) return true;
    m_cachedTree = 0;
    addAliasesToTTree(tree);
    for ( std::map<std::string,EventMap*>::iterator itr = m_evtMap.begin(); 
	  itr != m_evtMap.end(); itr++ ) {
      if ( itr->second == 0 ) return false;
      if ( ! itr->second->initializeShortCuts(tree) ) return false;
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


  void EventClass::StripLineBreaks(std::string& fromString) {
    size_t find = fromString.find('\n');
    while ( find != fromString.npos ) {
      fromString.replace(find,1," ");
      find = fromString.find('\n');
    }
    find = fromString.find("  ");
    while ( find != fromString.npos ) {
      fromString.replace(find,2," ");
      find = fromString.find("  ");
    }    
  }

  
  void EventClass::writePythonDict(std::ostream& os) {
    bool first = true;
    std::string varDef = "AliasDict_";
    varDef += m_version;
    varDef += " = {";
    std::string indent(varDef.size(),' ');
    os << varDef;
    if ( m_aliasDict ) {
      m_aliasDict->writePythonDict(os,indent);
      first = false;
    }
    for ( std::map<std::string,EventMap*>::const_iterator itr = m_evtMap.begin(); itr != m_evtMap.end(); itr++ ) {
      itr->second->writePythonDict(os,indent,first);
      first = false;
    }
    os << '}' << std::endl << std::endl;

    os << "def load_AliasDict_" << m_version << "(rootTree):" << std::endl
       << "  for (key,val) in AliasDict_" << m_version << ".items():" << std::endl
       << "    rootTree.SetAlias(key,val)" << std::endl
       << "    pass" << std::endl
       << "  return" << std::endl << std::endl;    
  }


#ifndef USE_ROOT_XML

  EventClass* EventClass::loadFromXml(const std::string& fileName) {
    
    static const std::string EventClass("EventClass");
    static const std::string AliasDict("AliasDict");  
    static const std::string Alias("Alias");  
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
    
    evtUtils::EventClass* evtClass = new evtUtils::EventClass(version);

    std::vector<DOMElement*> aliasDicts;
    xmlBase::Dom::getChildrenByTagName(top,AliasDict,aliasDicts);
    if ( aliasDicts.size() > 1 ) {
      delete doc;
      return 0;
    }
    
    DOMElement* aliasDict = aliasDicts.size() == 1 ? aliasDicts[0] : 0;
    if ( aliasDict ) {
      std::vector<DOMElement*> aliases;
      xmlBase::Dom::getChildrenByTagName(aliasDict,Alias,aliases);
      for ( std::vector<DOMElement*>::iterator itrAlias = aliases.begin(); itrAlias != aliases.end(); itrAlias++ ) {
	DOMElement* elemAlias = *itrAlias;
	std::string aliasName = xmlBase::Dom::getAttribute(elemAlias,"name");
	std::string aliasVal = xmlBase::Dom::getTextContent(elemAlias); 
	StripLineBreaks(aliasVal);
	evtClass->addAlias(aliasName,aliasVal);
      }
    }
    
    std::vector<DOMElement*> eventMaps;
    xmlBase::Dom::getChildrenByTagName(top,EventMap,eventMaps);
    
    for ( std::vector<DOMElement*>::iterator itrMap = eventMaps.begin(); itrMap != eventMaps.end(); itrMap++ ) {
      DOMElement* elemMap = *itrMap;
      std::string mapName = xmlBase::Dom::getAttribute(elemMap,"mapName");
      std::string altName = xmlBase::Dom::getAttribute(elemMap,"altName");
      evtUtils::EventMap* evtMap = evtClass->addEventMap(mapName,altName);
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
	StripLineBreaks(shortCut);
	std::string fullCut;
	std::string comment;
	DOMElement* elemFullCut = xmlBase::Dom::findFirstChildByName(elemCat,FullCut); 
	DOMElement* elemComment = xmlBase::Dom::findFirstChildByName(elemCat,Comment); 
	if ( elemFullCut ) {
	  fullCut = xmlBase::Dom::getTextContent(elemFullCut); 
	  StripLineBreaks(fullCut);
	}
	if ( elemComment ) {
	  comment = xmlBase::Dom::getTextContent(elemComment); 
	}
	evtUtils::EventCategory* evtCat = evtMap->addCateogry(catName,bitVal,shortCut,fullCut,comment);
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
    
    evtUtils::EventClass* evtClass = new evtUtils::EventClass(version);
  
    XMLNodePointer_t elemMap = theEngine.GetChild(top);
    while ( elemMap != 0 ) {
      const char* mapName = theEngine.GetAttr(elemMap,"mapName");
      const char* altName = theEngine.GetAttr(elemMap,"altName");
      evtUtils::EventMap* evtMap = evtClass->addEventMap(mapName,altName);
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
	evtUtils::EventCategory* evtCat = evtMap->addCateogry(catName,bitVal,shortCut,fullCut,comment);
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

  bool EventClass::writeToHtml(EventClass& evtClass,
			       std::ostream& os) {
    
    os << "    <table width=\"100%\" border=1>" << std::endl;
    os << "      <tbody>" << std::endl;
    std::list<std::string> evtMapNames;
    evtClass.getEvtMapNames(evtMapNames);
    for ( std::list<std::string>::const_iterator itrMap = evtMapNames.begin(); itrMap != evtMapNames.end(); itrMap++ ) {
      evtUtils::EventMap* evtMap = evtClass.getEventMapByName(*itrMap);
      if ( evtMap == 0 ) {
	return false;
      }
      os << "        <tr>" << std::endl;
      os << "          <td colspan=\"5\" bgcolor=\"lightblue\" align=\"center\">" << std::endl;
      os << "            <strong>" << evtMap->getMapName() << "</strong>" << std::endl;      
      os << "          </td>" << std::endl;
      os << "        </tr>" << std::endl;      
      os << "        <tr>" << std::endl;
      os << "          <td width=\"5%\">Bit</td>" << std::endl;
      os << "          <td width=\"10%\">Name</td>" << std::endl;
      os << "          <td width=\"25%\">Comment</td>" << std::endl;
      os << "          <td width=\"25%\">Short Version of Cut</td>" << std::endl;
      os << "          <td width=\"35%\">Full Version of Cut</td>" << std::endl;
      os << "        </tr>" << std::endl;
      for ( unsigned i(0); i < 32; i++ ) {
	evtUtils::EventCategory* evtCat = evtMap->getCutByIndex(i);
	if ( evtCat == 0 ) continue;
	os << "        <tr>" << std::endl;
	os << "          <td>" << i << "</td>" << std::endl;
	os << "          <td>" << evtCat->getName() << "</td>" << std::endl;
	os << "          <td>" << evtCat->getComment() << "</td>" << std::endl;
	os << "          <td>" << evtCat->getShortCut() << "</td>" << std::endl;
	os << "          <td>" << evtCat->getFullCut() << "</td>" << std::endl;
	os << "        </tr>" << std::endl;
      }
      os << "        </tr>" << std::endl;  
      os << "        <tr>" << std::endl;  
      os << "        </tr>" << std::endl;        
    }  
  
    os << "      </tbody>" << std::endl;
    os << "    </table>" << std::endl;
    return true;
  }
}
