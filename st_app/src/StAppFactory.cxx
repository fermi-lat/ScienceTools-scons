/** \file StAppFactory.cxx
    \brief Factory class for Science Tools application objects derived from StApp.
    \author James Peachey, HEASARC
*/

#include <stdexcept>

#include "hoops/hoops_exception.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_stream/Stream.h"
#include "st_stream/st_stream.h"

namespace st_app {

  // Static member definitions:
  IStAppFactory * IStAppFactory::s_factory = 0;

  // Singleton access.
  IStAppFactory & IStAppFactory::instance() {
    if (0 == s_factory) throw std::logic_error("IStAppFactory::instance() cannot find a factory");
    return *s_factory;
  }

  // Construct IStAppFactory, setting singleton if it's not already defined.
  IStAppFactory::IStAppFactory(): m_app_name(), m_debug_mode(false) { if (0 == s_factory) s_factory = this; }

  // Construct IStAppFactory, setting singleton if it's not already defined.
  IStAppFactory::IStAppFactory(const std::string & app_name): m_app_name(app_name), m_debug_mode(false) {
    if (0 == s_factory) s_factory = this;
  }

  // Destruct IStAppFactory, unsetting singleton if this is is.
  IStAppFactory::~IStAppFactory() throw() { if (this == s_factory) s_factory = 0; }

  void IStAppFactory::configureApp() {
    // Initialize standard st_stream objects.
    st_stream::OStream::initStdStreams();

    // If application has a name, use it to set up various standard global properties.
    if (!m_app_name.empty()) {
      // Pass the name to st_stream, to use in its prefixes.
      st_stream::SetExecName(m_app_name);
      try {
        // Get parameter object, ignoring problems.
        st_app::AppParGroup pars(m_app_name);
        try {
          // Try to get chatter parameter, and use it to set the maximum tool chatter. Ignore problems.
          int chat = pars["chatter"];
          st_stream::SetMaximumChatter(chat);
        } catch (hoops::Hexception &) {}
        try {
          // Try to get debug parameter, and use it to enable/disable debug behavior. Ignore problems.
          m_debug_mode = pars["debug"];
        } catch (hoops::Hexception &) {}
      } catch (hoops::Hexception &) {}
    }

    // Compile-time debugging trumps debug parameter.
#ifdef ST_APP_DEBUG
    m_debug_mode = true;
#endif

    // Set up streams for debugging.
    st_stream::SetDebugMode(m_debug_mode);
  }

  bool IStAppFactory::getDebugMode() const { return m_debug_mode; }
}
