/** \file StAppFactory.cxx
    \brief Factory class for Science Tools application objects derived from StApp.
    \author James Peachey, HEASARC
*/

#include <stdexcept>

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

namespace st_app {

  // Static member definitions:
  IStAppFactory * IStAppFactory::s_factory = 0;

  // Singleton access.
  IStAppFactory & IStAppFactory::instance() {
    if (0 == s_factory) throw std::logic_error("IStAppFactory::instance() cannot find a factory");
    return *s_factory;
  }

  // Construct IStAppFactory, setting singleton if it's not already defined.
  IStAppFactory::IStAppFactory() { if (0 == s_factory) s_factory = this; }

  // Destruct IStAppFactory, unsetting singleton if this is is.
  IStAppFactory::~IStAppFactory() throw() { if (this == s_factory) s_factory = 0; }

}
