/** \file STGMainFrame.cxx
    \brief Modified TGMainFrame for internal use by Root graphics classes.
    \author James Peachey, HEASARC/GSSC
*/
#include "TGClient.h"

#include "STGMainFrame.h"

namespace st_graph {

  STGMainFrame::STGMainFrame(unsigned int width, unsigned int height): TGMainFrame(gClient->GetRoot(), width, height) {
  }

  STGMainFrame::~STGMainFrame() {}

  void STGMainFrame::CloseWindow() {}
}
