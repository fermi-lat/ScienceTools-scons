#ifndef st_graph_RootPlot_h
#define st_graph_RootPlot_h

#include <string>

#include "st_graph/IPlot.h"

class TCanvas;
class TGraph;

namespace st_graph {

  class IPlotFrame;
  class ValueSet;

  class RootPlot : public IPlot {
    public:
      RootPlot(IPlotFrame * parent, const std::string & style, const std::string & title, const ValueSet & x, const ValueSet & y,
        const ValueSet & z = ValueSet());

      virtual ~RootPlot();

      /// \brief Display this plot.
      virtual void display();

      /// \brief Hide this plot.
      virtual void unDisplay();

      /** \brief Associate this plot with the given canvas. Not API call.
          \param canvas The canvas.
      */
      virtual void setCanvas(TCanvas * canvas);

    private:
      IPlotFrame * m_parent;
      TCanvas * m_canvas;
      TGraph * m_graph;
  };
}

#endif
