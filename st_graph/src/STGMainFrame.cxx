/** \file STGMainFrame.cxx
    \brief Modified TGMainFrame for internal use by Root graphics classes.
    \author James Peachey, HEASARC/GSSC
*/
#include "TApplication.h"
#include "TGClient.h"

#include "RootEngine.h"
#include "STGMainFrame.h"

namespace st_graph {

  STGMainFrame::STGMainFrame(RootEngine * engine, unsigned int width, unsigned int height):
    TGMainFrame(gClient->GetRoot(), width, height), m_engine(engine) {}

  void STGMainFrame::CloseWindow() {
    m_engine->hideFrames();
    gApplication->Terminate(0);
  }

}
