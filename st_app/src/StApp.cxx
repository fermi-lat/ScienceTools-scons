/** \file StApp.cxx
    \brief Base class for Science Tools applications.
    \author James Peachey, HEASARC
*/
#include <cctype>
#include <stdexcept>
#include <string>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StGui.h"

#include "st_stream/StreamFormatter.h"

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
  StApp::StApp(): m_name("Unknown application"), m_version("unknown"), m_par_group(0) {}

  // Destruct application object:
  StApp::~StApp() throw() { delete m_par_group; }

  void StApp::runGui() {
    StEventReceiver gui(st_graph::Engine::instance(), getParGroup(), this);
    gui.run();
  }

  // Return an object which provides the most typical kinds of parameter access.
  AppParGroup & StApp::getParGroup() {
    return getParGroup(m_name);
  }

  // Return an object which provides the most typical kinds of parameter access.
  AppParGroup & StApp::getParGroup(const std::string & app_name) {
    // Create if necessary:
    if (0 == m_par_group) m_par_group = new AppParGroup(app_name);
    return *m_par_group;
  }

  void StApp::banner() const {
    st_stream::StreamFormatter sf("StApp", "banner", 2);
    sf.info(0) << "This is " << m_name << " version " << m_version << std::endl;
  }

  const std::string & StApp::getName() const { return m_name; }

  const std::string & StApp::getVersion() const { return m_version; }

  void StApp::setName(const std::string & name) { m_name = name; }

  void StApp::setVersion(const std::string & version) {
    static const std::string cvs_prefix("$Name:");

    std::string::size_type start = version.find(cvs_prefix);

    // If cvs was used to assign the version automatically, chop out the prefix and suffix ($) it uses.
    if (std::string::npos != start) {
      start += cvs_prefix.size();

      // Skip whitespace after prefix.
      for (std::string::const_iterator itor = version.begin() + start; itor != version.end(); ++itor, ++start) {
        if (!isspace(*itor)) break;
      }

      // Stop when whitespace or trailing $ is reached.
      std::string::size_type stop = start;
      for (std::string::const_iterator itor = version.begin() + stop; itor != version.end(); ++itor, ++stop) {
        if (isspace(*itor) || '$' == *itor) break;
      }

      // String between start and stop is the version.
      m_version = version.substr(start, stop - start);

    } else {
      m_version = version;
    }
  }

}
