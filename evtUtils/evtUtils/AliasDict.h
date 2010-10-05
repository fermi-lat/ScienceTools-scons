//      -*- Mode: c++ -*-

/** @file AliasDict.h
@brief header file for AliasDict.cxx
@author Eric Charles

$Header$
*/


#ifndef EventUtils_AliasDict_h
#define EventUtils_AliasDict_h

#include <Rtypes.h>
#include <string>
#include <map>

class TTree;

/** @class AliasDict
@brief Keeps track of all the aliases

@author Eric Charles

*/

namespace evtUtils {

  class AliasDict {
    
  public:
    
    AliasDict()
      :m_cachedTree(0){
    }
    
    AliasDict(const std::string& version)
      :m_cachedTree(0){
    }
  
    virtual ~AliasDict();
  
    // Utility methods  
    void addAlias(const std::string& key, const std::string& val);
    
    bool addAliasesToTTree(TTree& tree);
        
    bool getAlias(const std::string& key, std::string& val);

    void writePythonDict(std::ostream& os, const std::string& indent);

    // accesss
    const TTree* getCachedTree() const { return m_cachedTree; }
    
  private:
    
    std::map<std::string,std::string>      m_aliasMap;   //!
    TTree*                                 m_cachedTree; //!

    //ClassDef(AliasDict,0) // Keeps track of a set of maps of cuts
    
  };

}

#endif
