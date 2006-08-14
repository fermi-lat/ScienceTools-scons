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

  class Color {
    public:
      enum Color_e { eWhite, eBlack, eRed, eGreen, eBlue, eYellow, eMagenta, eCyan, eNumberOfColors };

      static int nextColor(int current_color);
  };

  class Marker {
    public:

      Marker();

      Marker(double x, double y, const std::string & text, int color = Color::eBlack);

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
      virtual void addMarker(double x, double y, const std::string & text, int color = Color::eBlack) = 0;

      /** \brief Get container of labels.
          \param labels The output container of labels.
      */
      virtual void getMarkers(std::vector<Marker> & labels) const = 0;

      /** \brief Get the current color of line used to connect points in plot.
      */
      virtual int getLineColor() const = 0;

      /** \brief Set the color of line used to connect points in plot.
          \param color Integer code indicating desired color of line.
      */
      virtual void setLineColor(int color) = 0;

      /** \brief Get integer expressing the current style of line used to connect points in plot.
      */
      virtual std::string getLineStyle() const = 0;

      /** \brief Set the style of line used to connect points in plot.
          \param style String indicating type of line; may be none, solid, dotted, dashed.
      */
      virtual void setLineStyle(const std::string & style) = 0;

      /** \brief Get the current type of curve used to connect points in plot.
      */
      virtual std::string getCurveType() const = 0;

      /** \brief Set the type of curve used to connect data points.
          \param type String indicating type of connection: curve or line.
      */
      virtual void setCurveType(const std::string & type) = 0;
  };

}

#endif
