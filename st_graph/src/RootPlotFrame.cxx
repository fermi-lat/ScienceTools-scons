/** \file RootPlotFrame.cxx
    \brief Implementation for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/

#include <stdexcept>

#include "TCanvas.h"
#include "TMultiGraph.h"
#include "TRootEmbeddedCanvas.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

namespace st_graph {

  RootPlotFrame::RootPlotFrame(IFrame * parent, unsigned int width, unsigned int height):
    RootFrame(parent, 0), m_canvas(0), m_multi_graph(0) {
    
    // Hook together Root primitives.
    TGCompositeFrame * root_frame = dynamic_cast<TGCompositeFrame *>(m_parent->getTGFrame());
    if (0 == root_frame)
      throw std::logic_error("RootPlotFrame constructor was passed a parent frame which is not associated with a Root frame");

    m_canvas = new TRootEmbeddedCanvas("", root_frame, width, height);
    root_frame->AddFrame(m_canvas);
    m_canvas->GetCanvas()->SetFillColor(0);

    m_multi_graph = new TMultiGraph("tmultigraph", "TMultiGraph");

    m_frame = m_canvas;
  }

  // Note m_frame is deleted in the base class destructor.
  RootPlotFrame::~RootPlotFrame() { delete m_multi_graph; }

  void RootPlotFrame::addFrame(IFrame * frame) {
    RootPlot * root_plot = dynamic_cast<RootPlot*>(frame);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::addFrame was passed a frame which is not a Root plot");

    //m_multi_graph->Add(root_plot->getTGraph(), "L");

    RootFrame::addFrame(frame);
  }

  void RootPlotFrame::removeFrame(IFrame * frame) {
    RootPlot * root_plot = dynamic_cast<RootPlot*>(frame);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::removeFrame was passed a frame which is not a Root plot");

    RootFrame::removeFrame(frame);
  }

  void RootPlotFrame::display() {
    RootFrame::display();

    // Save current pad.
    TVirtualPad * save_pad = gPad;

    // Select embedded canvas for drawing.
    gPad = m_canvas->GetCanvas();

    m_multi_graph->Draw("A");

    // Update the display.
    gPad->Update();

    // Restore current pad.
    gPad = save_pad;

  }

  TMultiGraph * RootPlotFrame::getMultiGraph() { return m_multi_graph; }

}
