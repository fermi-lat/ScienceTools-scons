/** \file RootPlotHist.cxx
    \brief Implementation for Root plotter for all histograms.
    \author James Peachey, HEASARC/GSSC
*/
#include <stdexcept>

#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TPad.h"
#include "TRootEmbeddedCanvas.h"

#include "RootEngine.h"
#include "RootPlotHist.h"
#include "STGMainFrame.h"

namespace st_graph {

  RootPlotHist::RootPlotHist(RootEngine * engine, const std::string & title, unsigned int width, unsigned int height,
    const PlotHist::IntervalCont_t & intervals): PlotHist(title, width, height), m_engine(engine),
    m_x_intervals(0), m_y_intervals(0), m_main_frame(0), m_canvas(0), m_hist1(0), m_hist2(0) {
    // Set up main frame etc. (Root initialization stuff).
    init();

    // Copy/create intervals in a form which is acceptable to Root.
    m_x_intervals = createIntervals(intervals);

    // Create a histogram to contain the data.
    m_hist1 = new TH1D(m_title.c_str(), m_title.c_str(), intervals.size(), m_x_intervals);
  }

  RootPlotHist::RootPlotHist(RootEngine * engine, const std::string & title, unsigned int width, unsigned int height,
    const PlotHist::IntervalCont_t & x_intervals, const PlotHist::IntervalCont_t & y_intervals):
    PlotHist(title, width, height), m_engine(engine), m_x_intervals(0), m_y_intervals(0), m_main_frame(0), m_canvas(0),
    m_hist1(0), m_hist2(0) {
    // Set up main frame etc. (Root initialization stuff).
    init();

    // Copy/create intervals in a form which is acceptable to Root.
    m_x_intervals = createIntervals(x_intervals);
    m_y_intervals = createIntervals(y_intervals);

    // Create a histogram to contain the data.
    m_hist2 = new TH2D(m_title.c_str(), m_title.c_str(), x_intervals.size(), m_x_intervals, y_intervals.size(), m_y_intervals);
  }

  RootPlotHist::~RootPlotHist() {
    m_engine->removeFrame(this);
    delete m_hist2;
    delete m_hist1;
    delete [] m_y_intervals;
    delete [] m_x_intervals;
    delete m_canvas;
    delete m_main_frame;
  }

  void RootPlotHist::display() {
    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    // Draw the histogram.
    if (0 != m_hist1) m_hist1->Draw();
    else if (0 != m_hist2) m_hist2->Draw("SURF");

    // Update the display.
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;

    // Display the main frame and its canvas.
    m_main_frame->MapSubwindows();
    m_main_frame->Layout();
    m_main_frame->MapWindow();
  }

  void RootPlotHist::unDisplay() {
    // Unmap, but do not close window. After it is closed it cannot be referred to again.
    // However, is is safe to map the window if it's just been unmapped.
    m_main_frame->UnmapWindow();
  }

  void RootPlotHist::set(int index, double value) {
    if (0 == m_hist1) throw std::logic_error("RootPlotHist::set(index, value) was called for a non-1D histogram");
    // Root histograms start at 0 == underflow, and go through N + 1 == overflow, so add one to index.
    m_hist1->SetBinContent(index + 1, value);
  }

  void RootPlotHist::set(int x_index, int y_index, double value) {
    if (0 == m_hist2) throw std::logic_error("RootPlotHist::set(x_index, y_index, value) was called for a non-2D histogram");
    // Root histograms start at 0 == underflow, and go through N + 1 == overflow, so add one to each index.
    m_hist2->SetBinContent(x_index + 1, y_index + 1, value);
  }

  double * RootPlotHist::createIntervals(const PlotHist::IntervalCont_t & intervals) const {
    double * root_intervals = 0;
    // Render intervals in a Root-friendly form.
    int num_intervals = intervals.size();
    if (0 < num_intervals) {
      // Root intervals are a single C primitive array of doubles, with *one* extra bin for last bin.
      int num_root_intervals = num_intervals + 1;
      root_intervals = new double[num_root_intervals];

      // Take each Root interval to be the start point of the real interval.
      for (int ii = 0; ii < num_intervals; ++ii) {
        root_intervals[ii] = intervals[ii].first;
      }

      // Last Root interval is the end point of the last real interval.
      root_intervals[num_root_intervals - 1] = intervals[num_intervals - 1].second;
    } else throw std::logic_error("RootPlotHist::createIntervals was given invalid intervals");

    return root_intervals;
  }

  void RootPlotHist::init() {
    // Create main frame in which to display this plot.
    m_main_frame = new STGMainFrame(m_engine, m_width, m_height);

    // Create an embedded canvas object in which to draw the histogram.
    m_canvas = new TRootEmbeddedCanvas("", m_main_frame, m_width, m_height);

    // Display the canvas in the main frame.
    m_main_frame->AddFrame(m_canvas);

    // Register this object with its owner engine so the engine can hide/show it.
    m_engine->addFrame(this);
  }

  int RootPlotHist::dimensionality() const {
    if (m_hist2) return 2;
    return 1;
  }

}
