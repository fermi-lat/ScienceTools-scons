/** \file AppParGroup.h
    \brief Standard way to handle hoops.
    \author James Peachey, HEASARC
*/
#include <cctype>
#include <stdexcept>
#include <utility>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"

namespace st_app {

  // Construct parameter group, getting arguments from the application and calling hoops base class.
  AppParGroup::AppParGroup(const std::string & comp_name):
    hoops::ParPromptGroup(StApp::getArgc() - 1, StApp::getArgv() + 1, comp_name), m_switch(), m_case(), m_prompt_mode(true) {}

  // Copy construct parameter group.
  AppParGroup::AppParGroup(const AppParGroup & group): hoops::ParPromptGroup(group), m_switch(), m_case(),
    m_prompt_mode(group.m_prompt_mode) {}

  // Assignments, using hoops to do the real work.
  hoops::IParGroup & AppParGroup::operator =(const AppParGroup & group)
    { hoops::ParPromptGroup::operator =(group); return *this; }
   
  hoops::IParGroup & AppParGroup::operator =(const hoops::IParGroup & group)
    { hoops::ParPromptGroup::operator =(group); return *this; }

  void AppParGroup::Prompt() {
    if (m_prompt_mode) {
      if (m_case.empty()) { 
        hoops::ParPromptGroup::Prompt();
      } else {
        // Loop over all parameters in sequence.
        for (hoops::GenParItor itor = begin(); itor != end(); ++itor) {
          // Get name, and skip parameters without a name (comments and blanks)
          const std::string & par_name = (*itor)->Name();
          if (par_name.empty()) continue;

          // If no switches are involved, prompt for parameters.
          bool do_prompt = true;

          // Find out if this parameter depends on any switch/cases.
          CaseList case_cont;
          getCase(par_name, case_cont);
          for (CaseList::iterator case_itor = case_cont.begin(); case_itor != case_cont.end(); ++case_itor) {
            // Get the switched parameter.
            const std::string & switch_name = case_itor->first;
            hoops::IPar & switch_par = Find(switch_name);

            // Get the case value for the switch, and make it all upper case.
            const std::string & case_name = case_itor->second;
            std::string value = switch_par.Value();
            for (std::string::iterator s_itor = value.begin(); s_itor != value.end(); ++s_itor) *s_itor = toupper(*s_itor);

            // Suppress prompts if any of the switches are not equal to the case value.
            if (value != case_name) {
              do_prompt = false; // i.e. do_prompt &&= value == case_name;
              break;
            }
          }
          // Prompt unless it was suppressed by a switch.
          if (do_prompt) Prompt(par_name);
        }
      }
    }
  }

  void AppParGroup::Prompt(const std::string & par_name) {
    if (m_prompt_mode) hoops::ParPromptGroup::Prompt(par_name);
  }

  void AppParGroup::suppressPrompts(bool suppress) { m_prompt_mode = !suppress; }

  bool AppParGroup::isSwitch(const std::string & par_name) const {
    return (m_switch.end() != m_switch.find(par_name));
  }

  void AppParGroup::getCase(const std::string & par_name, CaseList & case_cont) const {
    case_cont.clear();
    std::pair<Case::const_iterator, Case::const_iterator> range = m_case.equal_range(par_name);
    for (Case::const_iterator itor = range.first; itor != range.second; ++itor) {
      case_cont.push_back(ParValuePair(itor->second.first, itor->second.second));
    }
  }

  void AppParGroup::setSwitch(const std::string & switch_name) {
    // See if this group contains the given "switch" parameter. This throws if there is no such parameter.
    Find(switch_name);

    // See if this switch was already added.
    Switch::iterator itor = m_switch.find(switch_name);

    // Insert a new (empty) case container for this switch.
    if (m_switch.end() == itor) m_switch.insert(itor, switch_name);
  }

  void AppParGroup::setCase(const std::string & switch_name, const std::string & case_name, const std::string & par_name) {
    // See if this switch was already added.
    Switch::iterator itor = m_switch.find(switch_name);

    // If case not present, throw an exception.
    if (m_switch.end() == itor)
      throw std::logic_error("setCase: switch parameter \"" + switch_name + "\" not found when trying to add case == \"" +
        case_name + "\"");

    if (case_name.empty()) throw std::logic_error("setCase called with empty case label");
    else if (par_name.empty()) throw std::logic_error("setCase called with empty parameter name");

    // Store case labels as all uppercase.
    std::string uc_case_name = case_name;
    for (std::string::iterator s_itor = uc_case_name.begin(); s_itor != uc_case_name.end(); ++s_itor) *s_itor = toupper(*s_itor);

    m_case.insert(std::make_pair(par_name, std::make_pair(switch_name, uc_case_name)));
  }
}
