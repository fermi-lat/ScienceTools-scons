/** \file RootPlot.cxx
    \brief Implementation of RootPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#include <cctype>
#include <stdexcept>
#include <vector>

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TPad.h"

#include "RootPlot.h"

namespace st_graph {

  RootPlot::RootPlot(IFrame * parent, const std::string & style, const std::string & title, const ValueSet & x,
    const ValueSet & y, const ValueSet & z): m_parent(parent), m_canvas(0), m_graph(0), m_hist(0) {

    // Sanity check.
    if (x.size() != y.size()) throw std::logic_error("RootPlot: x and y ValueSets do not have same size");

    // Convert style string to lower case.
    std::string lc_style = style;
    for (std::string::iterator itor = lc_style.begin(); itor != lc_style.end(); ++itor) *itor = tolower(*itor);

    if (z.empty()) {
      if (0 == lc_style.find("hist")) {
        m_hist = createHistPlot(title, x, y, z);
      } else if (0 == lc_style.find("scat")) {
        m_graph = createScatterPlot(title, x, y, z);
      } else {
        throw std::logic_error("RootPlot: unknown plot style \"" + style + "\"");
      }
    } else {
      throw std::logic_error("RootPlot: plots with more than 2 dimensions not yet supported");
    }
    if (0 != m_parent) m_parent->addFrame(this);
  }

  RootPlot::~RootPlot() {
    if (0 != m_parent) m_parent->removeFrame(this);
    delete m_graph;
    delete m_hist;
  }

  void RootPlot::display() {
    if (0 == m_canvas) throw std::logic_error("RootPlot::display is not attached to a RootPlotFrame");

    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    // Draw the graph.
    if (0 != m_graph) m_graph->Draw("ALP");
    if (0 != m_hist) m_hist->Draw("H");

    // Update the display.
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;

  }

  void RootPlot::unDisplay() {
    // ?
  }

  void RootPlot::addFrame(IFrame *) { throw std::logic_error("RootPlot::addFrame cannot add frames to a plot"); }
  void RootPlot::removeFrame(IFrame *) { throw std::logic_error("RootPlot::removeFrame cannot remove frames from a plot"); }

  long RootPlot::getL() const { return 0; }
  void RootPlot::setL(long) {}

  long RootPlot::getR() const { return 0; }
  void RootPlot::setR(long) {}

  void RootPlot::setCanvas(TCanvas * canvas) {
    m_canvas = canvas;
  }

  TH1 * RootPlot::createHistPlot(const std::string & title, const ValueSet & x, const ValueSet & y, const ValueSet & z) {
    TH1 * retval = 0;
    if (z.empty()) {
      // For convenience, make aliases to vectors containing bins and data.
      const std::vector<double> & bc(x.getValues()); // Bin centers.
      const std::vector<double> & bw(x.getSpreads()); // Bin widths.
      const std::vector<double> & dc(y.getValues()); // Data values.

      // Create new vectors which will hold the bins and data in the form Root TH1 wants.
      std::vector<double> bins(2 * x.size() + 1); // One extra bin for the end of the last interval.
      std::vector<double> data(2 * x.size());

      // Create indices to iterate through the bins and data.
      unsigned int in_idx = 0;
      unsigned int out_idx = 0;

      // Bin 0 is different, so it is handled specially outside the loop.
      // Bin 0 for Root TH1 is the underflow bin, which has 0 counts.
      data[out_idx] = 0.;

      // Bin 0 is the bin center minus the half-width.
      bins[out_idx] = bc[in_idx] - bw[in_idx] * .5;
      ++out_idx;

      // Iterate over input bins, stopping *before the last bin.
      for (;in_idx < x.size() - 1; ++in_idx, ++out_idx) {
        // Set current data.
        data[out_idx] = dc[in_idx];
        bins[out_idx] = bc[in_idx] + bw[in_idx] * .5;

        // See whether the lower bound of the next bin is distinct from the upper bound of the current bin.
        double next_bin = bc[in_idx + 1] - bw[in_idx + 1] * .5;
        if (next_bin > bins[out_idx]) {
          // There is a gap in the histogram, so add a bin with content == 0.
          ++out_idx;
          bins[out_idx] = next_bin;
          data[out_idx] = 0.;
        }
      }

      // Fill last data, and penultimate bin.
      data[out_idx] = dc[in_idx];
      bins[out_idx] = bc[in_idx] + bw[in_idx] * .5;

      // Last bin is the overflow bin. width is arbitrarily same as last bin width.
      bins[out_idx + 1] = bins[out_idx] + bw[in_idx] * .5;
      
      // Create the Root histogram, and fill it from the data.
      TH1D * hist = new TH1D(title.c_str(), title.c_str(), out_idx + 1, &*bins.begin());
      for (unsigned int ii = 0; ii < data.size(); ++ii) hist->SetBinContent(ii, data[ii]);

      retval = hist;

    } else {
      throw std::logic_error("RootPlot::createHistPlot: not supported for more than 2 dimensions");
    }
    return retval;
  }

  TGraph * RootPlot::createScatterPlot(const std::string & title, const ValueSet & x, const ValueSet & y, const ValueSet & z) {
    TGraph * retval = 0;
    // Get arrays of values.
    const double * x_pts = &*x.getValues().begin();
    const double * x_err = &*x.getSpreads().begin();
    const double * y_pts = &*y.getValues().begin();
    const double * y_err = &*y.getSpreads().begin();

    if (z.empty()) {
      // Create the graph.
      retval = new TGraphErrors(x.size(), x_pts, y_pts, x_err, y_err);
    } else {
      throw std::logic_error("RootPlot::createScatterPlot: not supported for more than 2 dimensions");
    }

    // Set title.
    retval->SetTitle(title.c_str());
    
    return retval;
  }

}
