/** \file RootPlotFrame.cxx
    \brief Implementation for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

#include "TAxis.h"
#include "TCanvas.h"
#include "TGFrame.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH2.h"
#include "TMultiGraph.h"
#include "TRootEmbeddedCanvas.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

namespace st_graph {

  RootPlotFrame::RootPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height,
    bool delete_parent): RootFrame(parent, 0, 0, delete_parent), m_plots(), m_tgraphs(), m_title(title), m_canvas(0),
    m_multi_graph(0), m_th2d(0), m_dimensionality(0) {
    
    // Hook together Root primitives.
    TGCompositeFrame * root_frame = dynamic_cast<TGCompositeFrame *>(m_parent->getTGFrame());
    if (0 == root_frame)
      throw std::logic_error("RootPlotFrame constructor was passed a parent frame which does not possess a Root frame");

    // Create Root TGCanvas which is suitable for embedding plots.
    // The canvas *is* the m_frame -- will be deleted by the base class destructor.
    m_canvas = new TRootEmbeddedCanvas(createRootName("TRootEmbeddedCanvas", this).c_str(), root_frame, width, height);

    root_frame->AddFrame(m_canvas);
    m_canvas->GetCanvas()->SetFillColor(0);

    m_frame = m_canvas;
  }

  // Note m_frame is deleted in the base class destructor.
  RootPlotFrame::~RootPlotFrame() {
    // Note: This appears more complicated than necessary, but be careful changing it. Under some circumstances,
    // a RootPlot needs to delete its parent, but the parent will always attempt to delete the RootPlot in the
    // process. Thus it is important to ensure the child is detached at the right times to prevent deleting
    // the parent and/or the child twice.

    unDisplay();
    // Delete children.
    while (!m_plots.empty()) {
      // Find last child.
      std::list<RootPlot *>::iterator itor = --m_plots.end();
      // Break links between this and the child.
      removePlot(*itor);
      // Delete the child.
      delete *itor;
    }
    // Delete Root widgets.
    delete m_multi_graph;
    delete m_th2d;
  }

  void RootPlotFrame::display() {
    RootFrame::display();

    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    try {
      // Modifying axes must be done through Root TAxis objects.
      std::vector<TAxis *> root_axes(3);

      // Display plot correctly for the current dimensionality. Get Root axes objects.
      if (m_dimensionality == 2) display2d(root_axes);
      else if (m_dimensionality == 3) display3d(root_axes);

      // Flags indicating whether titles have been set.
      std::vector<bool> title_set(3, false);

      // Get titles from IPlots.
      for (std::list<RootPlot *>::iterator itor = m_plots.begin(); itor != m_plots.end(); ++itor) {
        // Extract IPlot-style axes.
        const std::vector<Axis> & axes = (*itor)->getAxes();

        // Loop over all dimensions, setting titles as needed.
        for (unsigned int index = 0; index != m_dimensionality; ++index) {
          // If title was not already set, set it.
          const std::string & title(axes[index].getTitle());
          if (!title_set[index] && !title.empty()) {
            root_axes[index]->SetTitle(title.c_str());
            root_axes[index]->CenterTitle(kTRUE);
            title_set[index] = true;
          }
        }
      }

    } catch (...) {
      gPad = save_pad;
      throw;
    }

    // Force complete update of the display.
    gPad->Modified();
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;
  }

  void RootPlotFrame::unDisplay() {
    // Undisplay all Root children.
    for (std::list<TGraph *>::reverse_iterator itor = m_tgraphs.rbegin(); itor != m_tgraphs.rend(); ++itor) {
      if (0 != m_multi_graph) m_multi_graph->RecursiveRemove(*itor);
      delete *itor;
    }
    m_tgraphs.clear();
  
    RootFrame::unDisplay();
  }

  void RootPlotFrame::addPlot(IPlot * plot) {
    RootPlot * root_plot = dynamic_cast<RootPlot *>(plot);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::addPlot cannot add a non-Root plot");

    if (m_plots.empty()) m_dimensionality = root_plot->getDimensionality();
    else if (m_dimensionality != root_plot->getDimensionality())
      throw std::logic_error("RootPlotFrame::addPlot cannot overlay plots with different numbers of dimensions");

    // Make certain plot is not added more than once.
    if (m_plots.end() == std::find(m_plots.begin(), m_plots.end(), root_plot)) {
      m_plots.push_back(root_plot);
      root_plot->setParent(this);
    }
  }

  void RootPlotFrame::removePlot(IPlot * plot) {
    std::list<RootPlot *>::iterator itor = std::find(m_plots.begin(), m_plots.end(), plot);
    if (m_plots.end() != itor) {
      RootPlot * root_plot = dynamic_cast<RootPlot *>(plot);
      if (0 != root_plot) root_plot->setParent(0);
      m_plots.erase(itor);
    }
  }

  const std::string & RootPlotFrame::getTitle() const {
    return m_title;
  }

  void RootPlotFrame::display2d(std::vector<TAxis *> & axes) {
    axes.assign(3, 0);

    // Create or get parent multi-graph.
    getMultiGraph();

    for (std::list<RootPlot *>::iterator itor = m_plots.begin(); itor != m_plots.end(); ++itor) {
      // Get numeric sequences from data.
      const std::vector<const ISequence *> sequences((*itor)->getSequences());

      // Unpack the sequences: first dimension is the x axis, second is the y.
      const ISequence * x = sequences.at(0);
      const ISequence * y = sequences.at(1);

      // Determine the style of the graph.
      std::string style = (*itor)->getStyle();

      // Depending on the style, create appropriate Root plot object.
      TGraph * tgraph = 0;
      if (style == "hist")
        tgraph = createHistPlot(*x, *y);
      else
        tgraph = createScatterPlot(*x, *y);

      // Keep track of Root object, so it can be deleted later.
      m_tgraphs.push_back(tgraph);

      // Connect Root objects.
      m_multi_graph->Add(tgraph, "L");
    }

    // Draw parent TMultiGraph object.
    m_multi_graph->Draw("A");

    // Get axes. Set all three dimensions even though this is 2D.
    axes.resize(3);
    axes[0] = m_multi_graph->GetXaxis();
    axes[1] = m_multi_graph->GetYaxis();
    axes[2] = 0;
  }

  void RootPlotFrame::display3d(std::vector<TAxis *> & axes) {
    axes.assign(3, 0);

    if (m_plots.empty()) return;
    else if (1 != m_plots.size()) throw std::logic_error("RootPlotFrame::display3d overlaying 3d plots not supported");
    std::list<RootPlot *>::iterator itor = m_plots.begin();

    // Get numeric sequences from data.
    const std::vector<const ISequence *> sequences((*itor)->getSequences());

    // Unpack the sequences: first dimension is the x axis, second is the y.
    const ISequence * x = sequences.at(0);
    const ISequence * y = sequences.at(1);

    // Get data being plotted.
    const std::vector<std::vector<double> > & z((*itor)->getZData());

    // Create Root plotting object.
    m_th2d = createHistPlot2D(createRootName("TH2D", *itor), *x, *y, z);

    m_th2d->Draw("lego");

    // Get axes.
    axes[0] = m_th2d->GetXaxis();
    axes[1] = m_th2d->GetYaxis();
    axes[2] = m_th2d->GetZaxis();
  }

  TGraph * RootPlotFrame::createHistPlot(const ISequence & x, const ISequence & y) {
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

  TGraph * RootPlotFrame::createScatterPlot(const ISequence & x, const ISequence & y) {
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

  TH2D * RootPlotFrame::createHistPlot2D(const std::string & root_name, const ISequence & x, const ISequence & y,
    const std::vector<std::vector<double> > & z) {

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

    // Create the histogram used to draw the plot.
    hist = new TH2D(root_name.c_str(), getTitle().c_str(), x_bins.size() - 1, &x_bins[0], y_bins.size() - 1, &y_bins[0]);

    // Populate the histogram.
    for (unsigned int ii = 0; ii < x.size(); ++ii)
      for (unsigned int jj = 0; jj < y.size(); ++jj)
        hist->SetBinContent(ii + 1, jj + 1, z[ii][jj]);

    return hist;
  }

  std::string RootPlotFrame::createRootName(const std::string & prefix, void * ptr) const {
    // The Root name of the object (by which it may be looked up) is its address, converted 
    // to a string. This should prevent collisions.
    std::ostringstream os;
    os << prefix << " " << ptr;
    return os.str();
  }

  TMultiGraph * RootPlotFrame::getMultiGraph() {
    if (0 == m_multi_graph) {
      m_multi_graph = new TMultiGraph(createRootName("TMultiGraph", this).c_str(), m_title.c_str());
    }
    return m_multi_graph;
  }

}
