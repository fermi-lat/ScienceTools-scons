/** \file AppParGroup.h
    \brief Standard way to handle hoops.
    \author James Peachey, HEASARC
*/

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"

namespace st_app {

  // Construct parameter group, getting arguments from the application and calling hoops base class.
  AppParGroup::AppParGroup(const std::string & comp_name):
    hoops::ParPromptGroup(StApp::getArgc() - 1, StApp::getArgv() + 1, comp_name) {}

  // Copy construct parameter group.
  AppParGroup::AppParGroup(const AppParGroup & group): hoops::ParPromptGroup(group) {}

  // Assignments, using hoops to do the real work.
  hoops::IParGroup & AppParGroup::operator =(const AppParGroup & group)
    { hoops::ParPromptGroup::operator =(group); return *this; }
   
  hoops::IParGroup & AppParGroup::operator =(const hoops::IParGroup & group)
    { hoops::ParPromptGroup::operator =(group); return *this; }

}
