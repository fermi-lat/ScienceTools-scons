/** \file IApp.cxx
    \brief Base class for Science Tools applications.
    \author James Peachey, HEASARC
*/

#include <stdexcept>
#include <string>

#include "hoops/hoops.h"
#include "hoops/hoops_prompt_group.h"

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

  // Accessor for number of command line arguments.
  int IApp::getArgc() { return s_argc; }

  // Accessor for command line arguments.
  char ** IApp::getArgv() { return s_argv; }

  // Destructor:
  IApp::~IApp() throw() {
    delete m_hoops_par_group;
    // Make sure getApp will not try to return this application after it's gone:
    if (this == s_application) s_application = 0;
  }

  // Constructor:
  IApp::IApp(const std::string & app_name): m_app_name(app_name), m_hoops_par_group(0) {
    // It's OK in principle to have more than one application, but only one is the top-level singleton
    // object returned by getApp(). If no singleton application is already playing that role, this is it.
    if (0 == s_application) s_application = this;

  }

  // Get the name of this application.
  const std::string & IApp::name() const {
    if (m_app_name.empty() && 0 != s_argv) m_app_name = s_argv[0];
    return m_app_name;
  }

  // Methods which use hoops:
  // Return the group, creating it first if necessary.
  hoops::IParGroup & IApp::hoopsGetParGroup() {
    if (0 == m_hoops_par_group) m_hoops_par_group = new hoops::ParPromptGroup(s_argc, s_argv, name());
    return *m_hoops_par_group;
  }

  // Prompt for all parameters, in the order given in the .par file.
  void IApp::hoopsPrompt() {
    hoopsGetParGroup();
    m_hoops_par_group->Prompt();
  }

  // Save the current parameter set.
  void IApp::hoopsSave() {
    if (0 != m_hoops_par_group) m_hoops_par_group->Save();
  }

}
