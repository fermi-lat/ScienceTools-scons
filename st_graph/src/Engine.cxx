/** \file Engine.cxx
    \brief Implementation of class which encapsulates a particular graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#include <stdexcept>

#include "RootEngine.h"
#include "st_graph/Engine.h"

namespace st_graph {

  Engine::~Engine() {}

  Engine & Engine::instance() {
    static RootEngine s_engine;
    if (!s_engine.initSucceeded())
      throw std::runtime_error("Engine::instance() could not initialize graphical environment");
    return s_engine;
  }

  Engine::Engine() {}

}
