/** \file RootEngine.cxx
    \brief Implementation of class which encapsulates the Root graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#include "TApplication.h"
#include "TGClient.h"
#include "TGWindow.h"

#include "RootEngine.h"
#include "RootPlotHist1D.h"

#include "st_graph/PlotHist1D.h"

namespace st_graph {

  RootEngine::RootEngine() {
    // If no TApplication already exists, create one.
    if (0 == gApplication) {
      int argc = 0;
      gApplication = new TApplication("st_graph", &argc, 0);
    }
  }

  void RootEngine::run() {
    // Display all frames currently registered with this engine.
    for (FrameList_t::iterator itor = m_frames.begin(); itor != m_frames.end(); ++itor) (*itor)->display();

    // Run the Root event loop to handle the graphical displays.
    gApplication->Run(kTRUE);
  }

  PlotHist1D * RootEngine::createPlotHist1D(const std::string & title, unsigned int width, unsigned int height,
    const PlotHist1D::IntervalCont_t & intervals) {
    // Create Root-specific concrete object with the PlotHist1D interface.
    return new RootPlotHist1D(this, title, width, height, intervals);
  }

  void RootEngine::addFrame(PlotHist1D * frame) {
    m_frames.push_back(frame);
  }

  void RootEngine::removeFrame(PlotHist1D * frame) {
    m_frames.remove(frame);
  }

  void RootEngine::hideFrames() {
    // Undisplay all frames currently registered with this engine.
    for (FrameList_t::reverse_iterator itor = m_frames.rbegin(); itor != m_frames.rend(); ++itor) (*itor)->unDisplay();
  }

}
