/** \file RootPlot.cxx
    \brief Implementation of RootPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#include <cctype>
#include <stdexcept>
#include <vector>

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TH2.h"
#include "TMultiGraph.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

#include "st_graph/IFrame.h"

namespace st_graph {

  RootPlot::RootPlot(IFrame * parent, const std::string & style, const std::string & title, const ValueSet & x,
    const ValueSet & y, const ValueSet & z): m_parent(0), m_multi_graph(0), m_graph(0), m_th2d(0) {
// TODO 2. Alter RootPlotFrame::addFrame so it calls back to this class. That allows this class to call
//         TMultiGraph::Add(graph, opt) with display options when addFrame is called.
// TODO 3. Remove call to addFrame in the constructor?
// TODO 4. Change unDisplay to call TMultiGraph::RecursiveRemove or whatever.
    // Get the parent multi frame so that the plot can be added with desired style.
    RootPlotFrame * m_parent = dynamic_cast<RootPlotFrame *>(parent);
    if (0 == m_parent) throw std::logic_error("RootPlot constructor: parent must be a valid RootPlotFrame");

    // Sanity check.
    if (x.size() != y.size()) throw std::logic_error("RootPlot constructor: x and y ValueSets do not have same size");

    // Convert style string to lower case.
    std::string lc_style = style;
    for (std::string::iterator itor = lc_style.begin(); itor != lc_style.end(); ++itor) *itor = tolower(*itor);

    m_multi_graph = m_parent->getMultiGraph();

    if (0 == m_multi_graph) return;

    if (z.empty()) {
      if (std::string::npos != lc_style.find("hist")) {
        m_graph = createHistPlot(title, x, y, z);
        m_multi_graph->Add(m_graph, "L");
      } else if (std::string::npos != lc_style.find("scat")) {
        m_graph = createScatterPlot(title, x, y, z);
        m_multi_graph->Add(m_graph, "");
      } else {
        throw std::logic_error("RootPlot constructor: unknown plot style \"" + style + "\"");
      }
    } else {
      throw std::logic_error("RootPlot constructor: plots with more than 2 dimensions not yet supported");
    }
  }

  RootPlot::RootPlot(IFrame * parent, const std::string & style, const std::string & title, const ValueSet & x,
    const ValueSet & y, const std::vector<std::vector<double> > & z): m_parent(0), m_multi_graph(0), m_graph(0), m_th2d(0) {
    // Get the parent multi frame so that the plot can be added with desired style.
    RootPlotFrame * m_parent = dynamic_cast<RootPlotFrame *>(parent);
    if (0 == m_parent) throw std::logic_error("RootPlot constructor: parent must be a valid RootPlotFrame");

    // Sanity check.
    if (x.size() != z.size())
      throw std::logic_error("RootPlot constructor: x ValueSets and first data dimension do not have same size");
    if (y.size() != z.begin()->size())
      throw std::logic_error("RootPlot constructor: y ValueSets and second data dimension do not have same size");

    // Convert style string to lower case.
    std::string lc_style = style;
    for (std::string::iterator itor = lc_style.begin(); itor != lc_style.end(); ++itor) *itor = tolower(*itor);

    m_th2d = createHistPlot2D(title, x, y, z);
  }

  RootPlot::~RootPlot() {
    if (0 != m_multi_graph) m_multi_graph->RecursiveRemove(m_graph);
    delete m_th2d;
    delete m_graph;
  }

  void RootPlot::display() {}

  void RootPlot::unDisplay() {}

  TGraph * RootPlot::createHistPlot(const std::string & title, const ValueSet & x, const ValueSet & y, const ValueSet & z) {
    TGraph * retval = 0;
    // Get arrays of values.
    const std::vector<double> & x_pts = x.getValues();
    const std::vector<double> & x_err = x.getSpreads();
    const std::vector<double> & y_pts = y.getValues();
    //const std::vector<double> & y_err = y.getSpreads(); // ignored

    std::vector<double> x_vals(x_pts.size() * 4);
    std::vector<double> y_vals(x_pts.size() * 4);

    if (z.empty()) {
      // Use input arrays to create graphable data.
      unsigned long idx = 0;
      unsigned long ii = 0;
      x_vals[idx] = x_pts[ii] - x_err[ii] * .5;
      y_vals[idx] = 0.;
      ++idx;
      for (ii = 0; ii < x_pts.size(); ++ii, ++idx) {
        x_vals[idx] = x_pts[ii] - x_err[ii] * .5;
        y_vals[idx] = y_pts[ii];

        ++idx;
        x_vals[idx] = x_pts[ii] + x_err[ii] * .5;
        y_vals[idx] = y_pts[ii];

        if (ii != x_pts.size() - 1) {
          double next = x_pts[ii + 1] - x_err[ii + 1] * .5;

          if (next > x_vals[idx]) {
            ++idx;
            x_vals[idx] = x_vals[idx - 1];
            y_vals[idx] = 0.;
            ++idx;
            x_vals[idx] = next;
            y_vals[idx] = 0.;
          }
        } else {
          // Last input point.
          ++idx;
          x_vals[idx] = x_vals[idx - 1];
          y_vals[idx] = 0.;
        }
      }

      // Create the graph.
      retval = new TGraph(idx, &*x_vals.begin(), &*y_vals.begin());
      retval->SetEditable(kFALSE);
    } else {
      throw std::logic_error("RootPlot::createScatterPlot: not supported for more than 2 dimensions");
    }

    // Set title.
    retval->SetTitle(title.c_str());
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
      retval->SetEditable(kFALSE);
    } else {
      throw std::logic_error("RootPlot::createScatterPlot: not supported for more than 2 dimensions");
    }

    // Set title.
    retval->SetTitle(title.c_str());
    
    return retval;
  }

  TH2D * RootPlot::createHistPlot2D(const std::string & title, const ValueSet & x, const ValueSet & y,
    const std::vector<std::vector<double> > & z) {

    TH2D * hist = 0;

    // Set up x bins. There is one extra for Root's upper cutoff.
    std::vector<double> x_bins(x.size() + 1);

    // Use low edges of all bins.
    for (unsigned int ii = 0; ii < x.size(); ++ii) x_bins[ii] = x.getLowVal(ii);

    // Last bin is overflow.
    x_bins[x.size()] = x.getHighVal(x.size() - 1);

    // Set up y bins. There is one extra for Root's upper cutoff.
    std::vector<double> y_bins(y.size() + 1);

    // Use low edges of all bins.
    for (unsigned int ii = 0; ii < y.size(); ++ii) y_bins[ii] = y.getLowVal(ii);

    // Last bin is overflow.
    y_bins[y.size()] = y.getHighVal(y.size() - 1);

    // Create the histogram used to draw the plot.
    hist = new TH2D(title.c_str(), title.c_str(), x_bins.size() - 1, &x_bins[0], y_bins.size() - 1, &y_bins[0]);

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
