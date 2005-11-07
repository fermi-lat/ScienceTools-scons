/** \file IPlot.h
    \brief Interface for IPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_IPlot_h
#define st_graph_IPlot_h

#include <string>
#include <vector>

#include "st_graph/Axis.h"

namespace st_graph {

  class ISequence;

  class Marker {
    public:
      enum Color_e { WHITE, BLACK, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, NUMBER_OF_COLORS };

      Marker(double x, double y, const std::string & text, int color = BLUE);

      std::string m_text;
      double m_x;
      double m_y;
      int m_color;
  };

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

      /** \brief Add a label to this plot.
          \param x X coordinate of label.
          \param y Y coordinate of label.
          \param text Text to display in the label.
      */
      virtual void addMarker(double x, double y, const std::string & text, int color = Marker::BLUE) = 0;

      /** \brief Get container of labels.
          \param labels The output container of labels.
      */
      virtual void getMarkers(std::vector<Marker> & labels) const = 0;
  };

}

#endif
