/** \file RootPlotHist1D.cxx
    \brief Implementation for Root plotter for 1D histograms.
    \author James Peachey, HEASARC/GSSC
*/
#include <stdexcept>

#include "TCanvas.h"
#include "TH1D.h"
#include "TPad.h"
#include "TRootEmbeddedCanvas.h"

#include "RootEngine.h"
#include "RootPlotHist1D.h"
#include "STGMainFrame.h"

namespace st_graph {

  RootPlotHist1D::RootPlotHist1D(RootEngine * engine, const std::string & title, unsigned int width, unsigned int height,
    const PlotHist1D::IntervalCont_t & intervals): PlotHist1D(title, width, height, intervals), m_engine(engine),
    m_root_intervals(0), m_main_frame(0), m_canvas(0), m_hist(0) {
    // Register this object with its owner engine so the engine can hide/show it.
    m_engine->addFrame(this);

    // Save intervals in a Root-friendly form.
    int num_intervals = intervals.size();
    if (0 < num_intervals) {
      // Root intervals are a single C primitive array of doubles, with one extra value for the last bin.
      int num_root_intervals = num_intervals + 1;
      m_root_intervals = new double[num_root_intervals];

      // Take each Root interval to be the start point of the real interval.
      for (int ii = 0; ii < num_intervals; ++ii) {
        m_root_intervals[ii] = intervals[ii].first;
      }

      // Last Root interval is the end point of the last real interval.
      m_root_intervals[num_root_intervals - 1] = intervals[num_intervals - 1].second;
    } else throw std::logic_error("RootPlotHist1D constructor was given invalid intervals");

    // Create main frame in which to display this plot.
    m_main_frame = new STGMainFrame(m_engine, m_width, m_height);

    // Create an embedded canvas object in which to draw the histogram.
    m_canvas = new TRootEmbeddedCanvas("", m_main_frame, m_width, m_height);

    // Display the canvas in the main frame.
    m_main_frame->AddFrame(m_canvas);

    // Create a histogram to contain the data.
    m_hist = new TH1D(m_title.c_str(), m_title.c_str(), m_intervals.size(), m_root_intervals);
  }

  RootPlotHist1D::~RootPlotHist1D() {
    m_engine->removeFrame(this);
    delete m_hist;
    delete m_canvas;
    delete m_main_frame;
    delete [] m_root_intervals;
  }

  void RootPlotHist1D::display() {
    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    // Draw the histogram.
    m_hist->Draw();

    // Update the display.
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;

    // Display the main frame and its canvas.
    m_main_frame->MapSubwindows();
    m_main_frame->Layout();
    m_main_frame->MapWindow();
  }

  void RootPlotHist1D::unDisplay() {
    // Unmap, but do not close window. After it is closed it cannot be referred to again.
    // However, is is safe to map the window if it's just been unmapped.
    m_main_frame->UnmapWindow();
  }

  void RootPlotHist1D::set(int index, double value) {
    // Root histograms start at 0 == underflow, and go through N + 1 == overflow, so add one.
    m_hist->SetBinContent(index + 1, value);
  }
}
