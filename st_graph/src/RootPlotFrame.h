/** \file RootPlotFrame.h
    \brief Interface for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootPlotFrame_h
#define st_graph_RootPlotFrame_h

#include <set>

#include "RootFrame.h"

#include "st_graph/IPlotFrame.h"

class TGMainFrame;
class TRootEmbeddedCanvas;

namespace st_graph {

  class RootEngine;
  class RootFrame;

  /** \class RootPlotFrame
      \brief Interface for base class frame for all graphical frames.
  */
  class RootPlotFrame : public IPlotFrame, public RootFrame {
    public:
      typedef std::set<RootPlot *> PlotCont_t;

      RootPlotFrame(RootEngine * engine, IFrame * parent, unsigned int width, unsigned int height);

      /// \brief Destruct the frame.
      virtual ~RootPlotFrame();

      /// \brief Add a plot to the window. Not an API call.
      virtual void addPlot(IPlot * plot);

      /// \brief Remove a plot from the window. Not an API call.
      virtual void removePlot(IPlot * plot);

      virtual void display();

      virtual void unDisplay();

    private:
      PlotCont_t m_plot_cont;
      TRootEmbeddedCanvas * m_canvas;
  };

}

#endif
