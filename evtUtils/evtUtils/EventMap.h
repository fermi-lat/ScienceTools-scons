//      -*- Mode: c++ -*-

/** @file EventMap.h
@brief header file for EventMap.cxx
@author Eric Charles

$Header$
*/

#ifndef EventUtils_EventMap_h
#define EventUtils_EventMap_h

#include <Rtypes.h>
#include <string>
#include <map>

class TTreeFormula;
class TTree;

/** @class EventMap
@brief Maps a set of event selections in to bit map

@author Eric Charles

*/


namespace evtUtils {

  class EventCategory;

  class EventMap {
    
  public:
    
    EventMap(){}

    EventMap(const std::string& mapName, const std::string& altName)
      :m_mapName(mapName),
       m_altName(altName){
    }
    
    virtual ~EventMap();
    
    // Utility methods  
    EventCategory* addCateogry(const std::string& name, unsigned bit,
			       const std::string& shortCut, const std::string& fullCut, 
			       const std::string& comment);
    
    bool initializeShortCuts(TTree& tree);
    bool initializeFullCuts(TTree& tree);
    
    bool fillShortCutMap( );
    bool fillFullCutMap( );
    
    bool addAliasesToTTree(TTree& tree);
    
    EventCategory* getCutByName(const std::string& name);
    EventCategory* getCutByIndex(unsigned index);
    
    // Access Methods    
    const std::string& getMapName() const { return m_mapName; }
    const std::string& getAltName() const { return m_altName; }  
    
    UInt_t getShortMap() { return m_shortMap; }
    UInt_t getFullMap() { return m_fullMap; }

    UInt_t* getShortMapPtr() { return &m_shortMap; }
    UInt_t* getFullMapPtr() { return &m_fullMap; }

  private:
    
    std::string                             m_mapName;   //!
    std::string                             m_altName;   //!
    
    UInt_t                                  m_shortMap;  //!
    UInt_t                                  m_fullMap;   //!
    
    std::map<std::string,EventCategory*>    m_nameMap;   //!
    std::map<unsigned,EventCategory*>       m_indexMap;  //!

    ClassDef(EventMap,0) // Maps a set of cuts defined in xml to a bit field
    
  };

}

#endif
