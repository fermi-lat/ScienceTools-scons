/** \file RootPlotFrame.cxx
    \brief Implementation for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/

#include <stdexcept>

#include "TGClient.h"
#include "STGMainFrame.h"
#include "TRootEmbeddedCanvas.h"

#include "RootEngine.h"
#include "RootFrame.h"
#include "RootPlot.h"
#include "RootPlotFrame.h"

namespace st_graph {

  RootPlotFrame::RootPlotFrame(RootEngine * engine, IFrame * parent, unsigned int width, unsigned int height):
    RootFrame(engine, parent, 0), m_plot_cont(), m_canvas(0) {
    
    // Hook together Root primitives.
    TGCompositeFrame * root_frame = dynamic_cast<TGCompositeFrame *>(m_parent->getTGFrame());
    if (0 == root_frame)
      throw std::logic_error("RootPlotFrame constructor was passed a parent frame which is not associated with a Root frame");

    m_canvas = new TRootEmbeddedCanvas("", root_frame, width, height);
    root_frame->AddFrame(m_canvas);

    m_frame = m_canvas;
  }

  RootPlotFrame::~RootPlotFrame() {}

  void RootPlotFrame::display() {
    for (PlotCont_t::iterator itor = m_plot_cont.begin(); itor != m_plot_cont.end(); ++itor) (*itor)->display();
    RootFrame::display();
  }

  void RootPlotFrame::unDisplay() {
    RootFrame::unDisplay();
  }

  void RootPlotFrame::addPlot(IPlot * plot) {
    RootPlot * root_plot = dynamic_cast<RootPlot*>(plot);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::addPlot was passed a non-Root plot");

    m_plot_cont.insert(root_plot);

    root_plot->setCanvas(m_canvas->GetCanvas());
  }

  void RootPlotFrame::removePlot(IPlot * plot) {
    RootPlot * root_plot = dynamic_cast<RootPlot*>(plot);
    if (0 == root_plot) throw std::logic_error("RootPlotFrame::removePlot was passed a non-Root plot");

    root_plot->setCanvas(0);

    m_plot_cont.erase(m_plot_cont.find(root_plot));
  }

}
