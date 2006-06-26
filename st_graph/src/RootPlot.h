/** \file RootPlot.h
    \brief Interface of RootPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootPlot_h
#define st_graph_RootPlot_h

#include <string>
#include <vector>

#include "st_graph/Axis.h"
#include "st_graph/IPlot.h"
#include "st_graph/Sequence.h"

namespace st_graph {

  class IFrame;
  class ISequence;
  class RootPlotFrame;

  /** \class RootPlot
      \brief A Root implementation of the IFrame interface. This specific class displays a plot in its frame.
  */
  class RootPlot : public IPlot {
    public:
      /** \brief Construct a RootPlot object.
          \param parent The parent frame.
          \param style The style of the plot. Currently, either hist* or scat*, case insensitive, will produce
                 a histogram or scatter plot, respectively.
          \param x The first dimension.
          \param y The second dimension.
          \param delete_parent Flag indicating plot owns (and should delete) parent.
      */
      RootPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y, bool delete_parent = false);

      /** \brief Construct a RootPlot object.
          \param parent The parent frame.
          \param style The style of the plot.
          \param x The first dimension.
          \param y The second dimension.
          \param z The third dimension.
          \param delete_parent Flag indicating plot owns (and should delete) parent.
      */
      RootPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y,
        const std::vector<std::vector<double> > & z, bool delete_parent = false);

      virtual ~RootPlot();

      /// \brief Get the sequences this plot represents.
      virtual const std::vector<const ISequence *> getSequences() const;

      /// \brief Get the data represented by this plot. If plot does not have this type of data an exception will be thrown.
      virtual const std::vector<std::vector<double> > & getZData() const;

      /// \brief Get the number of dimensions of the plot, currently either 2 or 3.
      virtual unsigned int getDimensionality() const { return m_dimensionality; }

      /// \brief Get this plot's axes objects, with modification rights.
      virtual std::vector<Axis> & getAxes();

      /// \brief Get this plot's axes objects, without modification rights.
      virtual const std::vector<Axis> & getAxes() const;

      /** \brief Add a marker with optional text to this plot.
          \param x X coordinate of label.
          \param y Y coordinate of label.
          \param text Text to display in the label.
      */
      virtual void addMarker(double x, double y, const std::string & text, int color = Marker::BLUE);

      /** \brief Get container of labels.
          \param labels The output container of labels.
      */
      virtual void getMarkers(std::vector<Marker> & labels) const;

      virtual std::vector<Marker> & getMarkers();

      /** \brief Get the current style of line used to connect points in plot.
      */
      virtual std::string getLineStyle() const;

      /** \brief Set the style of line used to connect points in plot.
          \param style String indicating type of line; may be none, solid, dotted, dashed.
      */
      virtual void setLineStyle(const std::string & style);

      /** \brief Get the current type of curve used to connect points in plot.
      */
      virtual std::string getCurveType() const;

      /** \brief Set the type of curve used to connect data points.
          \param type String indicating type of connection: curve or line.
      */
      virtual void setCurveType(const std::string & type);

      /// \brief Return a string describing the plot style, e.g. hist, scat, lego, surf, etc.
      const std::string & getStyle() const;

      /** \brief Set the plot style.
          \param style The new style. This string will be converted to all lower case, and rationalized.
      */
      void setStyle(const std::string & style);

      /** \brief Set the frame which is parent to this plot.
          \param parent The new parent.
      */
      void setParent(RootPlotFrame * parent);

    private:
      std::vector<const ISequence *> m_seq_cont;
      std::vector<Marker> m_label;
      std::string m_style;
      std::string m_line_style;
      std::string m_curve_type;
      unsigned int m_dimensionality;
      RootPlotFrame * m_parent;
      const std::vector<std::vector<double> > * m_z_data;
      bool m_delete_parent;
  };

}

#endif
