// -*- Mode: c++ -*-


// This Class's header
#include "evtUtils/AliasDict.h"


#include <TTree.h>
#include <iostream>



namespace evtUtils {

  AliasDict::~AliasDict() {
  }
  
  void AliasDict::addAlias(const std::string& key, const std::string& val) {
    if ( m_aliasMap.count(key) != 0 ) return;
    m_aliasMap[key] = val;
  }
  
  bool AliasDict::addAliasesToTTree(TTree& tree) {
    if ( &tree == m_cachedTree ) return true;   
    for ( std::map<std::string,std::string>::iterator itr = m_aliasMap.begin(); 
	  itr != m_aliasMap.end(); itr++ ) {
      tree.SetAlias(itr->first.c_str(),itr->second.c_str());
    }
    m_cachedTree = &tree;
    return true;    
  }
  
  
  bool AliasDict::getAlias(const std::string& key, std::string& val) {
    std::map<std::string,std::string>::iterator itrFind = m_aliasMap.find(key);
    if ( itrFind == m_aliasMap.end() ) return false;
    val = itrFind->second;
    return true;
  }


  void AliasDict::writePythonDict(std::ostream& os, const std::string& indent) {
    bool first(true);
    for ( std::map<std::string,std::string>::const_iterator itr = m_aliasMap.begin(); itr != m_aliasMap.end(); itr++ ) {
      if ( first ) {
	first = false;
      } else {
	// terminate previous line and index
	os << ",\\" << std::endl << indent;
      }
      os << '"' << itr->first << "\":\"" << itr->second << '"';
    }
  }
  
  
}
