//      -*- Mode: c++ -*-

/** @file EventClass.h
@brief header file for EventClass.cxx
@author Eric Charles

$Header$
*/


#ifndef EventUtils_EventClass_h
#define EventUtils_EventClass_h

#include <Rtypes.h>
#include <string>
#include <map>
#include <list>

class TTree;

/** @class EventClass
@brief Keeps track of all the event class maps

@author Eric Charles

*/

namespace evtUtils {

  class EventMap;
  class AliasDict;

  class EventClass {
    
  public:
    
    static EventClass* loadFromXml(const std::string& fileName);
    
    static bool writeToHtml(EventClass& evtClass, 
			    std::ostream& os);

    static void StripLineBreaks(std::string& fromString);

  public:
    
    EventClass()
      :m_cachedTree(0),
       m_aliasDict(0){
    }
    
    EventClass(const std::string& version)
      :m_version(version),
       m_cachedTree(0),
       m_aliasDict(0){
    }
  
    virtual ~EventClass();
  
    // Utility methods  
    EventMap* addEventMap(const std::string& mapName, const std::string& altName);
    
    void addAlias(const std::string& aliasName, const std::string& aliasVal);

    bool addAliasesToTTree(TTree& tree);
    
    bool addShortBranchesToTTree(TTree& tree);
    
    bool addFullBranchesToTTree(TTree& tree);

    bool initializeShortCuts(TTree& tree);
    
    bool initializeFullCuts(TTree& tree);
    
    bool initializeBoth(TTree& tree);

    bool fillShortCutMaps( );
    
    bool fillFullCutMaps( );
    
    EventMap* getEventMapByName(const std::string& name);

    void getEvtMapNames(std::list<std::string>& names);
    
    UInt_t* getShortMapPtr(const std::string& name);

    UInt_t* getFullMapPtr(const std::string& name);

    // accesss
    const std::string& getVersion() const { return m_version; }
    const TTree* getCachedTree() const { return m_cachedTree; }
    inline const AliasDict* getAliasDict() const { return m_aliasDict; }
    inline AliasDict* getAliasDict() { return m_aliasDict; }    

    void writePythonDict(std::ostream& os);    

  private:
    
    std::string                            m_version;    //!
    std::map<std::string,EventMap*>        m_evtMap;     //!
    TTree*                                 m_cachedTree; //!
    AliasDict*                             m_aliasDict;  //!

    //ClassDef(EventClass,0) // Keeps track of a set of maps of cuts
    
  };

}

#endif
