/** \file IPlot.h
    \brief Interface for IPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_IPlot_h
#define st_graph_IPlot_h
#include <vector>

#include "st_graph/Axis.h"

namespace st_graph {

  class ISequence;

  /** \class IPlot
      \brief Base class for all plots which can be displayed in a plotting frame.
  */
  class IPlot {
    public:
      /// \brief Destruct the plotter.
      virtual ~IPlot();

      /// \brief Get the sequences this plot represents.
      virtual const std::vector<const ISequence *> getSequences() const = 0;

      /// \brief Get this plot's axes objects, with modification rights.
      virtual std::vector<Axis> & getAxes() = 0;

      /// \brief Get this plot's axes objects, without modification rights.
      virtual const std::vector<Axis> & getAxes() const = 0;

  };

}

#endif
