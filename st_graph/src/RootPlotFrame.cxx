/** \file RootPlotFrame.cxx
    \brief Implementation for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#include <sstream>
#include <stdexcept>

#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TRootEmbeddedCanvas.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

namespace st_graph {

  RootPlotFrame::RootPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height):
    RootFrame(parent, 0), m_title(title), m_canvas(0), m_multi_graph(0) {
    
    // Hook together Root primitives.
    TGCompositeFrame * root_frame = dynamic_cast<TGCompositeFrame *>(m_parent->getTGFrame());
    if (0 == root_frame)
      throw std::logic_error("RootPlotFrame constructor was passed a parent frame which is not associated with a Root frame");

    // The Root name of the object (by which it may be looked up) is its address, converted 
    // to a string. This should prevent collisions.
    std::ostringstream os;
    os << this;
    m_canvas = new TRootEmbeddedCanvas(os.str().c_str(), root_frame, width, height);

    root_frame->AddFrame(m_canvas);
    m_canvas->GetCanvas()->SetFillColor(0);

    m_frame = m_canvas;
  }

  // Note m_frame is deleted in the base class destructor.
  RootPlotFrame::~RootPlotFrame() { delete m_multi_graph; }

  void RootPlotFrame::display() {
    RootFrame::display();

    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    if (0 != m_multi_graph) m_multi_graph->Draw("A");

    // Update the display.
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;

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
