//      -*- Mode: c++ -*-
#ifndef evtUtils_EventCategory_h
#define evtUtils_EventCategory_h

/** @file EventCategory.h
@brief header file for EventCategory.cxx
@author Eric Charles

$Header$
*/

#include <Rtypes.h>
#include <string>

class TTreeFormula;
class TTree;

/** @class EventCategory
@brief Wraps a selection for a particular set of events

@author Eric Charles

*/

namespace evtUtils {

  class EventCategory {
    
  public:

    enum EventReturnCode { Error,
			   NoCut,
			   FailedCut,
			   PassedCut };
  
  public:

    EventCategory()
      :m_shortFormula(0),
       m_fullFormula(0){
    }

    EventCategory(const std::string& name, unsigned bit,
		  const std::string& shortCut, const std::string& fullCut, 
		  const std::string& comment)
      :m_name(name),
       m_shortCut(shortCut),
       m_fullCut(fullCut),
       m_comment(comment),
       m_bit(bit),
       m_shortFormula(0),
       m_fullFormula(0){
    }
    
    virtual ~EventCategory();
    
    // Utility methods
    TTreeFormula* initializeShortCut(TTree& tree);
    TTreeFormula* initializeFullCut(TTree& tree);
    
    void addAliasesToTTree(TTree& tree);
    
    EventReturnCode passesShortCut( ) const {
      return evaluateFormula(m_shortFormula);
    }
    
    EventReturnCode passesFullCut( ) const {
      return evaluateFormula(m_fullFormula);
    }
    
    // Access functions
    inline const std::string& getName() const { return m_name; }
    inline const std::string& getShortCut() const { return m_shortCut; }
    inline const std::string& getFullCut() const { return m_fullCut; }
    inline const std::string& getComment() const { return m_comment; }
    inline unsigned getBit() const { return m_bit; }
    
  private:
    
    EventReturnCode evaluateFormula(TTreeFormula*) const;
    
    std::string   m_name;           //!
    std::string   m_shortCut;       //!
    std::string   m_fullCut;        //!
    std::string   m_comment;        //!
    unsigned      m_bit;            //!
    
    TTreeFormula* m_shortFormula;   //!
    TTreeFormula* m_fullFormula;    //!
    
    ClassDef(EventCategory,0) // Implements a single cut defined in xml

  };

}

#endif
