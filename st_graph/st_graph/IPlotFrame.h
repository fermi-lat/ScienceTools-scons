/** \file IPlotFrame.h
    \brief Interface for IPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_IPlotFrame_h
#define st_graph_IPlotFrame_h

#include "st_graph/IFrame.h"

namespace st_graph {

  class IPlot;

  /** \class IPlotFrame
      \brief Interface for base class frame for all graphical frames.
  */
  class IPlotFrame {
    public:
      /// \brief Destruct the frame.
      virtual ~IPlotFrame() {}

      virtual void addPlot(IPlot * plot) = 0;

      virtual void removePlot(IPlot * plot) = 0;

  };

}

#endif
