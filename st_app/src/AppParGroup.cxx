/** \file AppParGroup.h
    \brief Standard way to handle hoops.
    \author James Peachey, HEASARC
*/

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"

namespace st_app {

  // Construct parameter group, getting arguments from the application and calling hoops base class.
  AppParGroup::AppParGroup(const std::string & comp_name):
    hoops::ParPromptGroup(StApp::getArgc() - 1, StApp::getArgv() + 1, comp_name), m_prompt_mode(true) {}

  // Copy construct parameter group.
  AppParGroup::AppParGroup(const AppParGroup & group): hoops::ParPromptGroup(group), m_prompt_mode(group.m_prompt_mode) {}

  // Assignments, using hoops to do the real work.
  hoops::IParGroup & AppParGroup::operator =(const AppParGroup & group)
    { hoops::ParPromptGroup::operator =(group); return *this; }
   
  hoops::IParGroup & AppParGroup::operator =(const hoops::IParGroup & group)
    { hoops::ParPromptGroup::operator =(group); return *this; }

  void AppParGroup::Prompt() {
    if (m_prompt_mode) hoops::ParPromptGroup::Prompt();
  }

  void AppParGroup::Prompt(const std::string & par_name) {
    if (m_prompt_mode) hoops::ParPromptGroup::Prompt(par_name);
  }

  void AppParGroup::suppressPrompts(bool suppress) { m_prompt_mode = !suppress; }

}
