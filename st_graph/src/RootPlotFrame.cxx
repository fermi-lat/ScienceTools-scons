/** \file RootPlotFrame.cxx
    \brief Implementation for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/

#include <stdexcept>

#include "TRootEmbeddedCanvas.h"

#include "RootPlot.h"
#include "RootPlotFrame.h"

namespace st_graph {

  RootPlotFrame::RootPlotFrame(IFrame * parent, unsigned int width, unsigned int height):
    RootFrame(parent, 0), m_canvas(0) {
    
    // Hook together Root primitives.
    TGCompositeFrame * root_frame = dynamic_cast<TGCompositeFrame *>(m_parent->getTGFrame());
    if (0 == root_frame)
      throw std::logic_error("RootPlotFrame constructor was passed a parent frame which is not associated with a Root frame");

    m_canvas = new TRootEmbeddedCanvas("", root_frame, width, height);
    root_frame->AddFrame(m_canvas);

    m_frame = m_canvas;
  }

  RootPlotFrame::~RootPlotFrame() {}

  void RootPlotFrame::addFrame(IFrame * frame) {
    RootPlot * root_plot = dynamic_cast<RootPlot*>(frame);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::addFrame was passed a frame which is not a Root plot");

    root_plot->setCanvas(m_canvas->GetCanvas());
    RootFrame::addFrame(frame);
  }

  void RootPlotFrame::removeFrame(IFrame * frame) {
    RootPlot * root_plot = dynamic_cast<RootPlot*>(frame);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::removeFrame was passed a frame which is not a Root plot");

    RootFrame::removeFrame(frame);
    root_plot->setCanvas(0);
  }

}
