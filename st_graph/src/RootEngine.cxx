/** \file RootEngine.cxx
    \brief Implementation of class which encapsulates the Root graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#include <cstdlib>
#include <csignal>
#include <stdexcept>
#include <vector>

#include <iostream>

#include "TApplication.h"
#include "TGClient.h"
#include "TGWindow.h"
#include "TSystem.h"

#include "st_graph/IFrame.h"

#include "RootEngine.h"
#include "RootFrame.h"
#include "RootPlot.h"
#include "RootPlotHist.h"
#include "RootPlotFrame.h"
#include "STGMainFrame.h"

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
      // Ignore signals when creating the application.
      std::vector<sighandler_t> handlers(16);
      for (int ii = 0; ii < 16; ++ii) {
        handlers[ii] = signal(ii, SIG_IGN);
      }

      // Create application.
      int argc = 0;
      gApplication = new TApplication("st_graph", &argc, 0);

      // Restore signal handlers.
      for (int ii = 0; ii < 16; ++ii) {
        signal(ii, handlers[ii]);
      }

      // Now test for success: if virtual X was set up
      // correctly, gClient will be non-0.
      if (0 == gClient)
        throw std::runtime_error("RootEngine::RootEngine could not create Root graphical TApplication");
    }
  }

  void RootEngine::run() {
    // Display all frames currently registered with this engine.
    for (FrameList_t::iterator itor = m_frames.begin(); itor != m_frames.end(); ++itor) (*itor)->display();

    // Run the Root event loop to handle the graphical displays.
    gApplication->Run(kTRUE);
  }

  IFrame * RootEngine::createMainFrame(unsigned int width, unsigned int height) {
    return new RootFrame(this, RootFrame::ancestor(), new STGMainFrame(this, width, height));
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

  IPlot * RootEngine::createPlot(IPlotFrame * parent, const std::string & style, const std::string & title, const ValueSet & x,
    const ValueSet & y, const ValueSet & z) {
    return new RootPlot(parent, style, title, x, y, z);
  }

  IPlotFrame * RootEngine::createPlotFrame(IFrame * parent, unsigned int width, unsigned int height) {
    return new RootPlotFrame(this, parent, width, height);
  }

  void RootEngine::addFrame(IFrame * frame) {
    m_frames.push_back(frame);
  }

  void RootEngine::removeFrame(IFrame * frame) {
    m_frames.remove(frame);
  }

  void RootEngine::hideFrames() {
    // Undisplay all frames currently registered with this engine.
    for (FrameList_t::reverse_iterator itor = m_frames.rbegin(); itor != m_frames.rend(); ++itor) (*itor)->unDisplay();
  }

}
