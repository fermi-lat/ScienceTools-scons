/** \file Engine.cxx
    \brief Implementation of class which encapsulates a particular graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#include "RootEngine.h"
#include "st_graph/Engine.h"

namespace st_graph {

  Engine * Engine::m_engine = 0;

  Engine::~Engine() {}

  Engine & Engine::instance() {
    // The first time this is called, create the engine.
    if (0 == m_engine) m_engine = new RootEngine;
    return *m_engine;
  }

  Engine::Engine() {}

}
