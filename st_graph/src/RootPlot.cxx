/** \file RootPlot.cxx
    \brief Implementation of RootPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#include <cctype>
#include <stdexcept>
#include <vector>

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

#include "st_graph/IFrame.h"

namespace st_graph {

  RootPlot::RootPlot(IFrame * parent, const std::string & style, const std::string & title, const ValueSet & x,
    const ValueSet & y, const ValueSet & z): m_parent(parent), m_graph(0) {
// TODO 1. Make m_parent a RootPlotFrame, do the cast one time here.
// TODO 2. Alter RootPlotFrame::addFrame so it calls back to this class. That allows this class to call
//         TMultiGraph::Add(graph, opt) with display options when addFrame is called.
// TODO 3. Remove call to addFrame in the constructor?
// TODO 4. Change unDisplay to call TMultiGraph::RecursiveRemove or whatever.
    if (0 == m_parent) throw std::logic_error("RootPlot: cannot create a plot without a parent window");

    // Sanity check.
    if (x.size() != y.size()) throw std::logic_error("RootPlot: x and y ValueSets do not have same size");

    // Convert style string to lower case.
    std::string lc_style = style;
    for (std::string::iterator itor = lc_style.begin(); itor != lc_style.end(); ++itor) *itor = tolower(*itor);

    // Get the parent multi frame so that the plot can be added with desired style.
    RootPlotFrame * frame = dynamic_cast<RootPlotFrame *>(m_parent);
    if (0 == frame) throw std::logic_error("RootPlot: parent must be a RootPlotFrame");

    TMultiGraph * multi_graph = frame->getMultiGraph();

    if (z.empty()) {
      if (0 == lc_style.find("hist")) {
        m_graph = createHistPlot(title, x, y, z);
        multi_graph->Add(m_graph, "L");
      } else if (0 == lc_style.find("scat")) {
        m_graph = createScatterPlot(title, x, y, z);
        multi_graph->Add(m_graph, "");
      } else {
        throw std::logic_error("RootPlot: unknown plot style \"" + style + "\"");
      }
    } else {
      throw std::logic_error("RootPlot: plots with more than 2 dimensions not yet supported");
    }
    m_parent->addFrame(this);
  }

  RootPlot::~RootPlot() {
    m_parent->removeFrame(this);
    delete m_graph;
  }

  void RootPlot::display() {}

  void RootPlot::unDisplay() { m_parent->removeFrame(this); }

  void RootPlot::addFrame(IFrame *) { throw std::logic_error("RootPlot::addFrame cannot add frames to a plot"); }
  void RootPlot::removeFrame(IFrame *) { throw std::logic_error("RootPlot::removeFrame cannot remove frames from a plot"); }

  long RootPlot::getL() const { return 0; }
  void RootPlot::setL(long) {}

  long RootPlot::getR() const { return 0; }
  void RootPlot::setR(long) {}

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

  TGraph * RootPlot::getTGraph() { return m_graph; }

}
