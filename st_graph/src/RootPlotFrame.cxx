/** \file RootPlotFrame.cxx
    \brief Implementation for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#include <sstream>
#include <stdexcept>

#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TRootEmbeddedCanvas.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

namespace st_graph {

  RootPlotFrame::RootPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height):
    RootFrame(parent, 0), m_plots(), m_title(title), m_canvas(0), m_multi_graph(0) {
    
    // Hook together Root primitives.
    TGCompositeFrame * root_frame = dynamic_cast<TGCompositeFrame *>(m_parent->getTGFrame());
    if (0 == root_frame)
      throw std::logic_error("RootPlotFrame constructor was passed a parent frame which does not possess a Root frame");

    // The Root name of the object (by which it may be looked up) is its address, converted 
    // to a string. This should prevent collisions.
    std::ostringstream os;
    os << this;

    // Create Root TGCanvas which is suitable for embedding plots.
    // The canvas *is* the m_frame -- will be deleted by the base class destructor.
    m_canvas = new TRootEmbeddedCanvas(os.str().c_str(), root_frame, width, height);

    root_frame->AddFrame(m_canvas);
    m_canvas->GetCanvas()->SetFillColor(0);

    m_frame = m_canvas;
  }

  // Note m_frame is deleted in the base class destructor.
  RootPlotFrame::~RootPlotFrame() {
    // Delete children. When they are deleted, they will remove themselves from the container of plots.
    while (!m_plots.empty()) delete *--m_plots.end();
    delete m_multi_graph;
  }

  void RootPlotFrame::display() {
    RootFrame::display();

    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    // Display all children.
    for (std::list<RootPlot *>::iterator itor = m_plots.begin(); itor != m_plots.end(); ++itor) (*itor)->display();
  
    if (0 != m_multi_graph) m_multi_graph->Draw("A");

    // Update the display.
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;

  }

  void RootPlotFrame::unDisplay() {
    // Undisplay all children.
    for (std::list<RootPlot *>::reverse_iterator itor = m_plots.rbegin(); itor != m_plots.rend(); ++itor) {
      (*itor)->unDisplay();
    }
  
    RootFrame::unDisplay();
  }

  void RootPlotFrame::addPlot(IPlot * plot) {
    RootPlot * root_plot = dynamic_cast<RootPlot *>(plot);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::addPlot cannot add a non-Root plot");

    // Make certain plot is not added more than once.
    if (m_plots.end() == std::find(m_plots.begin(), m_plots.end(), root_plot)) {
      m_plots.push_back(root_plot);
    }
  }

  void RootPlotFrame::removePlot(IPlot * plot) {
    std::list<RootPlot *>::iterator itor = std::find(m_plots.begin(), m_plots.end(), plot);
    if (m_plots.end() != itor) {
      m_plots.erase(itor);
    }
  }

  TMultiGraph * RootPlotFrame::getMultiGraph() {
    if (0 == m_multi_graph) {
      // The Root name of the object (by which it may be looked up) is its address, converted 
      // to a string. This should prevent collisions.
      std::ostringstream os;
      os << this;
      m_multi_graph = new TMultiGraph(os.str().c_str(), m_title.c_str());
    }
    return m_multi_graph;
  }

  const std::string & RootPlotFrame::getTitle() const {
    return m_title;
  }

}
