/** \file IPlot.h
    \brief Interface for IPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_IPlot_h
#define st_graph_IPlot_h

#include <string>
#include <utility>
#include <vector>

#include "st_graph/ValueSet.h"

namespace st_graph {

  /** \class IPlot
      \brief Base class for all plots which can be displayed in a plotting frame.
  */
  class IPlot {
    public:
      /// \brief Destruct the plotter.
      virtual ~IPlot();

      /// \brief Display this plot.
      virtual void display() = 0;

      /// \brief Hide this plot.
      virtual void unDisplay() = 0;
  };

}

#endif
