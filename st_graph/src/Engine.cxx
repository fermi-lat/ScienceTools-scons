/** \file Engine.cxx
    \brief Implementation of class which encapsulates a particular graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#include "RootEngine.h"
#include "st_graph/Engine.h"

namespace st_graph {

  Engine::~Engine() {}

  Engine & Engine::instance() {
    static RootEngine s_engine;
    return s_engine;
  }

  Engine::Engine() {}

}
