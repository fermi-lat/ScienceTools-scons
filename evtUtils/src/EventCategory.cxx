// -*- Mode: c++ -*-


// This Class's header
#include "evtUtils/EventCategory.h"

#include <TTree.h>
#include <TTreeFormula.h>

#include<iostream>

//ClassImp(evtUtils::EventCategory);

namespace evtUtils {

  EventCategory::~EventCategory(){
    delete m_shortFormula;
    delete m_fullFormula;
  }
  
  TTreeFormula* EventCategory::initializeShortCut(TTree& tree) {
    delete m_shortFormula;
    std::string formulaName(m_name+"_Cut");
    m_shortFormula = new TTreeFormula(formulaName.c_str(),m_shortCut.c_str(),&tree);
    if ( m_shortFormula && m_shortFormula->GetNdim() == 0 ) {
      std::cerr << "Failed to compile formula:" << std::endl;
      m_shortFormula->Print();
      delete m_shortFormula;
      m_shortFormula = 0;
    }
    return m_shortFormula;
  }
  
  TTreeFormula* EventCategory::initializeFullCut(TTree& tree){
    delete m_fullFormula;
    if ( m_fullCut.empty() ) {
      m_fullFormula = 0;
      return m_fullFormula;
    }
    std::string formulaName(m_name+"_FullCut");
    m_fullFormula = new TTreeFormula(formulaName.c_str(),m_fullCut.c_str(),&tree);
    if ( m_fullFormula && m_fullFormula->GetNdim() == 0 ) {
      std::cerr << "Failed to compile formula:" << std::endl;
      m_fullFormula->Print();
      delete m_fullFormula;
      m_fullFormula = 0;
    }
    return m_fullFormula;
  }
  
  void EventCategory::addAliasesToTTree(TTree& tree) {
    tree.SetAlias(m_name.c_str(),m_shortCut.c_str());
    if ( m_fullCut.empty() ) {
      return;
    }
    std::string fullAliasName(m_name + "_Full");
    tree.SetAlias(fullAliasName.c_str(),m_fullCut.c_str());
    return;
  }
  
  EventCategory::EventReturnCode EventCategory::evaluateFormula(TTreeFormula* form) const {
    if ( form == 0 ) return EventCategory::NoCut;
    if ( form->GetTree() == 0 ) return EventCategory::Error;
    if ( form->GetTree()->GetReadEntry() < 0 ) { 
      // This is actually ok when writing the file.
      ;
    }
    // Eval instance return a double, for bool the values are 0. and 1.
    return form->EvalInstance() > 0.5 ? EventCategory::PassedCut : EventCategory::FailedCut;
  }

  void EventCategory::writePythonDict(std::ostream& os) {
    os << '"' << m_name << "\":\"" << m_shortCut << '"';
  }
  

}
