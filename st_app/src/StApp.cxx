/** \file StApp.cxx
    \brief Base class for Science Tools applications.
    \author James Peachey, HEASARC
*/

#include <stdexcept>
#include <string>

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

}
