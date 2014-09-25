/** \file MPLPlotFrame.h
    \brief Interface for MPLPlotFrame class.
    \author Tom Stephens, HEASARC/GSSC
*/
#ifndef st_graph_MPLPlotFrame_h
#define st_graph_MPLPlotFrame_h

#include "st_graph/MPLFrame.h"

#include <list>
#include <string>
#include <vector>

#include "st_graph/Axis.h"

//class TAxis;
//class TGraph;
//class TH2D;
//class TMultiGraph;

namespace st_graph {

  class IFrame;
  class IPlot;
  class ISequence;
  class Marker;
  class MPLPlot;
//  class StEmbeddedCanvas;

  /** \class MPLPlotFrame
      \brief A MPL frame which is suitable for displaying plots.
  */
  class MPLPlotFrame : public MPLFrame {
    public:
      /** \brief Construct a frame connected to the given parent, with the given properties.
          \param parent The parent frame.
          \param title The title to display on the frame.
          \param width The width of the frame in pixels.
          \param height The height of the frame in pixels.
          \param delete_parent Flag indicating frame owns (and should delete) parent.
      */
      MPLPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height,
        bool delete_parent = false);

      /// \brief Destruct the frame.
      virtual ~MPLPlotFrame();

      /// \brief Display this frame and all it contains.
      virtual void display();

      /// \brief Hide this frame and all it contains.
      virtual void unDisplay();

      virtual void reset();

      /** \brief Add the given plot to the frame.
          \param plot The plot to add.
      */
      virtual void addPlot(IPlot * plot);

      /** \brief Remove the given plot from the frame.
          \param plot The plot to remove. If not already in the frame, no harm done.
      */
      virtual void removePlot(IPlot * plot);

      virtual void addMarker(Marker & marker);

      /** \brief Get the title of the frame.
      */
      const std::string & getTitle() const;

      std::vector<Axis> & getAxes();

      const std::vector<Axis> & getAxes() const;

    protected:
      /** \brief Internal helper method which correctly displays 2d plots.
      */
      virtual void display2d();

      /** \brief Internal helper method which correctly displays 3d plots.
      */
      virtual void display3d();

      /** \brief Internal helper method which creates histogram plot as a MPL object.
          \param x The first dimension.
          \param y The second dimension.
          \param format matplotlib format string for line/marker type and color
      */
      virtual PyObject * createHistPlot(const ISequence & x, const ISequence & y,std::string format);

      /** \brief Internal helper method which creates scatter plot as a MPL object.
          \param x The first dimension.
          \param y The second dimension.
          \param format matplotlib format string for line/marker type and color
      */
      virtual PyObject * createScatterPlot(const ISequence & x, const ISequence & y,std::string format);

      /** \brief Internal helper method which creates 2d plot as a MPL object.
          \param root_name The name given to the created MPL object. Should be unique to avoid warnings from MPL.
          \param x The first dimension.
          \param y The second dimension.
	  \param z The third dimension.
      */
      virtual PyObject * createHistPlot2D(const std::string & root_name, const ISequence & x, const ISequence & y,
        const std::vector<std::vector<double> > & z);

      /** \brief Internal helper method which creates a name for MPL objects from the given prefix and a pointer.
          \param prefix String prefix for the MPL object.
	  \param ptr A pointer which will be concatenated with the prefix to form the name.
      */
      virtual std::string createRootName(const std::string & prefix, void * ptr) const;

      /// \brief Get the underlying MPL graphical object; create it if it does not yet exist.
//      virtual PyObject * getMultiGraph();

      /** \brief Generate a matplotlib format string for the line color, style and markers
       *
       *  \param plot The plot to work with
       */
      virtual std::string generateFormatString(MPLPlot * plot) const;

      /** \brief Generate matplotlib format color string for the specified colors
       *
       *  \param the Color enum value of the color desired
       */
      std::string getColorString(int color) const;

    private:
      std::vector<Axis> m_axes;
      std::list<MPLPlot *> m_plots;
      std::list<PyObject *> m_graphs;
      std::string m_title;
      PyObject * m_canvas;
      PyObject * m_multi_graph;
      PyObject * m_th2d;
      unsigned int m_dimensionality;
  };

}

#endif
