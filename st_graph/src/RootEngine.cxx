/** \file RootEngine.cxx
    \brief Implementation of class which encapsulates the Root graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#include "TApplication.h"
#include "TGClient.h"
#include "TGWindow.h"
#include "TSystem.h"

#include "RootEngine.h"
#include "RootPlotHist.h"

namespace st_graph {

  RootEngine::RootEngine() {
    gSystem->ResetSignal(kSigBus);
    gSystem->ResetSignal(kSigSegmentationViolation);
    gSystem->ResetSignal(kSigSystem);
    gSystem->ResetSignal(kSigPipe);
    gSystem->ResetSignal(kSigIllegalInstruction);
    gSystem->ResetSignal(kSigQuit);
    gSystem->ResetSignal(kSigInterrupt);
    gSystem->ResetSignal(kSigWindowChanged);
    gSystem->ResetSignal(kSigAlarm);
    gSystem->ResetSignal(kSigChild);
    gSystem->ResetSignal(kSigUrgent);
    gSystem->ResetSignal(kSigFloatingException);
    gSystem->ResetSignal(kSigTermination);
    gSystem->ResetSignal(kSigUser1);
    gSystem->ResetSignal(kSigUser2);

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

  PlotHist * RootEngine::createPlotHist1D(const std::string & title, unsigned int width, unsigned int height,
    const PlotHist::IntervalCont_t & intervals) {
    // Create Root-specific 1D plot object with the PlotHist interface.
    return new RootPlotHist(this, title, width, height, intervals);
  }

  PlotHist * RootEngine::createPlotHist2D(const std::string & title, unsigned int width, unsigned int height,
    const PlotHist::IntervalCont_t & x_intervals, const PlotHist::IntervalCont_t & y_intervals) {
    // Create Root-specific 2D plot object with the PlotHist interface.
    return new RootPlotHist(this, title, width, height, x_intervals, y_intervals);
  }

  void RootEngine::addFrame(PlotHist * frame) {
    m_frames.push_back(frame);
  }

  void RootEngine::removeFrame(PlotHist * frame) {
    m_frames.remove(frame);
  }

  void RootEngine::hideFrames() {
    // Undisplay all frames currently registered with this engine.
    for (FrameList_t::reverse_iterator itor = m_frames.rbegin(); itor != m_frames.rend(); ++itor) (*itor)->unDisplay();
  }

}
