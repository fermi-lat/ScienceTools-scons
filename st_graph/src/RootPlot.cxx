#include <cctype>
#include <stdexcept>

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TPad.h"

#include "RootPlot.h"

#include "st_graph/IPlotFrame.h"

namespace st_graph {

  RootPlot::RootPlot(IPlotFrame * parent, const std::string & style, const std::string & title, const ValueSet & x,
    const ValueSet & y, const ValueSet & z): m_parent(parent), m_canvas(0), m_graph(0) {

    // Sanity check.
    if (x.size() != y.size()) throw std::logic_error("RootPlot: x and y ValueSets do not have same size");

    // Convert style string to lower case.
    std::string lc_style = style;
    for (std::string::iterator itor = lc_style.begin(); itor != lc_style.end(); ++itor) *itor = tolower(*itor);

    if (z.empty()) {
      // 2D case.
      const double * x_pts = &*x.getValues().begin();
      const double * x_err = &*x.getSpreads().begin();
      const double * y_pts = &*y.getValues().begin();
      const double * y_err = &*y.getSpreads().begin();

      // Create the graph.
      m_graph = new TGraphErrors(x.size(), x_pts, y_pts, x_err, y_err);

      // Set title.
      m_graph->SetTitle(title.c_str());

    }
    if (0 == m_graph) throw std::logic_error("RootPlot: could not create a plot for style " + style);
    if (0 != m_parent) m_parent->addPlot(this);
  }

  RootPlot::~RootPlot() {
    if (0 != m_parent) m_parent->removePlot(this);
    delete m_graph;
  }

  void RootPlot::display() {
    if (0 == m_canvas) throw std::logic_error("RootPlot::display is not attached to a RootPlotFrame");

    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    // Draw the graph.
    m_graph->Draw("ALP");

    // Update the display.
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;

  }

  void RootPlot::unDisplay() {
    // ?
  }

  void RootPlot::setCanvas(TCanvas * canvas) {
    m_canvas = canvas;
  }

}
