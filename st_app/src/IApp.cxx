/** \file IApp.cxx
    \brief Base class for Science Tools applications.
    \author James Peachey, HEASARC
*/

#include <stdexcept>
#include <string>

#include "st_app/IApp.h"

namespace st_app {

  // Static member definitions:
  IApp * IApp::s_application = 0;
  int IApp::s_argc = 0;
  char ** IApp::s_argv = 0;

  // Singleton access to the main application:
  IApp * IApp::getApp() {
    if (0 == s_application)
      throw std::logic_error("No application defined. Client must declare a static instance of subclass of IApp.");
    return s_application;
  }

  // Process (for now simply store) command line arguments:
  void IApp::processCommandLine(int argc, char ** argv) {
    s_argc = argc;
    s_argv = argv;
  }

  // Destructor:
  IApp::~IApp() throw() {
    // Make sure getApp will not try to return this application after it's gone:
    if (this == s_application) s_application = 0;
  }

  // Constructor:
  IApp::IApp(const std::string & app_name): m_app_name(app_name) {
    // It's OK in principle to have more than one application, but only one is the top-level singleton
    // object returned by getApp(). If no singleton application is already playing that role, this is it.
    if (0 == s_application) s_application = this;

  }

}
