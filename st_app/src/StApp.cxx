/** \file StApp.cxx
    \brief Base class for Science Tools applications.
    \author James Peachey, HEASARC
*/

#include <stdexcept>
#include <string>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"

namespace st_app {

  // Static member definitions:
  int StApp::s_argc = 0;
  char ** StApp::s_argv = 0;

  // Process (for now simply store) command line arguments:
  void StApp::processCommandLine(int argc, char ** argv) {
    s_argc = argc;
    s_argv = argv;
  }

  // Accessor for number of command line arguments.
  int StApp::getArgc() { return s_argc; }

  // Accessor for command line arguments.
  char ** StApp::getArgv() { return s_argv; }

  // Construct application object:
  StApp::StApp(): m_par_group(0) {}

  // Destruct application object:
  StApp::~StApp() throw() { delete m_par_group; }

  // Return an object which provides the most typical kinds of parameter access.
  AppParGroup & StApp::getParGroup(const std::string & app_name) {
    // Create if necessary:
    if (0 == m_par_group) m_par_group = new AppParGroup(app_name);
    return *m_par_group;
  }

}
