/** \file RootPlot.cxx
    \brief Implementation of RootPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TGraphErrors.h"
#include "TH2.h"
#include "TMultiGraph.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

#include "st_graph/IFrame.h"
#include "st_graph/Sequence.h"

namespace st_graph {

  RootPlot::RootPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y): m_parent(0),
    m_multi_graph(0), m_graph(0), m_th2d(0) {
// TODO 2. Alter RootPlotFrame::addFrame so it calls back to this class. That allows this class to call
//         TMultiGraph::Add(graph, opt) with display options when addFrame is called.
// TODO 3. Remove call to addFrame in the constructor?
// TODO 4. Change unDisplay to call TMultiGraph::RecursiveRemove or whatever.
    // Get the parent multi frame so that the plot can be added with desired style.
    m_parent = dynamic_cast<RootPlotFrame *>(parent);
    if (0 == m_parent) throw std::logic_error("RootPlot constructor: parent must be a valid RootPlotFrame");

    // Sanity check.
    if (x.size() != y.size()) throw std::logic_error("RootPlot constructor: x and y sequence do not have same size");

    // Convert style string to lower case.
    std::string lc_style = style;
    for (std::string::iterator itor = lc_style.begin(); itor != lc_style.end(); ++itor) *itor = tolower(*itor);

    m_multi_graph = m_parent->getMultiGraph();

    if (0 == m_multi_graph) return;

    if (std::string::npos != lc_style.find("hist")) {
      m_graph = createHistPlot(x, y);
      m_multi_graph->Add(m_graph, "L");
    } else if (std::string::npos != lc_style.find("scat")) {
      m_graph = createScatterPlot(x, y);
      m_multi_graph->Add(m_graph, "L");
    } else {
      throw std::logic_error("RootPlot constructor: unknown plot style \"" + style + "\"");
    }
  }

  RootPlot::RootPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y,
    const std::vector<std::vector<double> > & z): m_parent(0), m_multi_graph(0), m_graph(0), m_th2d(0) {
    // Get the parent multi frame so that the plot can be added with desired style.
    m_parent = dynamic_cast<RootPlotFrame *>(parent);
    if (0 == m_parent) throw std::logic_error("RootPlot constructor: parent must be a valid RootPlotFrame");

    // Sanity check.
    if (x.size() != z.size())
      throw std::logic_error("RootPlot constructor: x sequence and first data dimension do not have same size");
    if (y.size() != z.begin()->size())
      throw std::logic_error("RootPlot constructor: y sequence and second data dimension do not have same size");

    // Convert style string to lower case.
    std::string lc_style = style;
    for (std::string::iterator itor = lc_style.begin(); itor != lc_style.end(); ++itor) *itor = tolower(*itor);

    m_th2d = createHistPlot2D(x, y, z);
  }

  RootPlot::~RootPlot() {
    if (0 != m_multi_graph) m_multi_graph->RecursiveRemove(m_graph);
    delete m_th2d;
    delete m_graph;
  }

  void RootPlot::display() {}

  void RootPlot::unDisplay() {}

  TGraph * RootPlot::createHistPlot(const ISequence & x, const ISequence & y) {
    TGraph * retval = 0;

    // Get arrays of values.
    std::vector<double> x_low;
    std::vector<double> x_high;
    std::vector<double> y_value;

    // Interpret x as a set of intervals.
    x.getIntervals(x_low, x_high);

    // Interpret y as the value in each interval.
    y.getValues(y_value);

    // Combine ranges and values into one array for axis and one array for the data; needed for TGraph.
    std::vector<double> x_vals(x_low.size() * 4);
    std::vector<double> y_vals(x_low.size() * 4);

    // Use input arrays to create graphable data.
    unsigned long idx = 0;
    unsigned long ii = 0;

    // First point plotted is at the base of the first bin.
    x_vals[idx] = x_low[ii];
    y_vals[idx] = 0.;

    ++idx;
    for (ii = 0; ii < x_low.size(); ++ii, ++idx) {
      // Plot the y value at the left edge.
      x_vals[idx] = x_low[ii];
      y_vals[idx] = y_value[ii];

      // Next plot the y value at the right edge.
      ++idx;
      x_vals[idx] = x_high[ii];
      y_vals[idx] = y_value[ii];

      // Exclude the last bin, which requires special handling.
      if (ii != x_low.size() - 1) {
        // See if the next bin's left edge is > than the right edge which was just plotted.
        double next = x_low[ii + 1];
        if (next > x_vals[idx]) {
          // There is a gap in the data, so plot a value of 0. in the gap.
          ++idx;
          x_vals[idx] = x_vals[idx - 1];
          y_vals[idx] = 0.;
          ++idx;
          x_vals[idx] = next;
          y_vals[idx] = 0.;
        }
      } else {
        // Last input point is the last right edge, which should drop to 0.
        ++idx;
        x_vals[idx] = x_vals[idx - 1];
        y_vals[idx] = 0.;
      }
    }

    // Create the graph.
    retval = new TGraph(idx, &*x_vals.begin(), &*y_vals.begin());

    retval->SetEditable(kFALSE);

    return retval;
  }

  TGraph * RootPlot::createScatterPlot(const ISequence & x, const ISequence & y) {
    TGraph * retval = 0;
    // Get arrays of values.
    std::vector<double> x_pts;
    std::vector<double> x_low_err;
    std::vector<double> x_high_err;
    std::vector<double> y_pts;
    std::vector<double> y_low_err;
    std::vector<double> y_high_err;

    x.getValues(x_pts);
    x.getSpreads(x_low_err, x_high_err);

    y.getValues(y_pts);
    y.getSpreads(y_low_err, y_high_err);

    // Create the graph.
    retval = new TGraphAsymmErrors(x.size(), &x_pts[0], &y_pts[0], &x_low_err[0], &x_high_err[0], &y_low_err[0], &y_high_err[0]);
    retval->SetEditable(kFALSE);

    return retval;
  }

  TH2D * RootPlot::createHistPlot2D(const ISequence & x, const ISequence & y, const std::vector<std::vector<double> > & z) {

    TH2D * hist = 0;

    typedef std::vector<double> Vec_t;

    // Set up x bins. There is one extra for Root's upper cutoff.
    Vec_t x_bins(x.size() + 1);

    // Set up intervals, big enough to hold either x or y bins.
    Vec_t lower(std::max(x.size(), y.size()));
    Vec_t upper(std::max(x.size(), y.size()));
    
    // Get intervals of x axis.
    x.getIntervals(lower, upper);

    // Use low edges of input sequence for all but the last bin.
    for (Vec_t::size_type ii = 0; ii < x.size(); ++ii) x_bins[ii] = lower[ii];

    // Last bin is taken from upper bound of last element in sequence.
    x_bins[x.size()] = upper[x.size() - 1];

    // Set up y bins. There is one extra for Root's upper cutoff.
    Vec_t y_bins(y.size() + 1);

    // Get intervals of y axis.
    y.getIntervals(lower, upper);

    // Use low edges of all bins.
    for (Vec_t::size_type ii = 0; ii < y.size(); ++ii) y_bins[ii] = lower[ii];

    // Last bin is overflow.
    y_bins[y.size()] = upper[y.size() - 1];

    // The Root name of the object (by which it may be looked up) is its address, converted 
    // to a string. This should prevent collisions.
    std::ostringstream os;
    os << this;

    // Create the histogram used to draw the plot.
    hist = new TH2D(os.str().c_str(), m_parent->getTitle().c_str(), x_bins.size() - 1, &x_bins[0], y_bins.size() - 1, &y_bins[0]);

    // Populate the histogram.
    for (unsigned int ii = 0; ii < x.size(); ++ii)
      for (unsigned int jj = 0; jj < y.size(); ++jj)
        hist->SetBinContent(ii + 1, jj + 1, z[ii][jj]);

    // Draw the histogram.
    hist->Draw("lego");

    return hist;
  }

  TGraph * RootPlot::getTGraph() { return m_graph; }

}
