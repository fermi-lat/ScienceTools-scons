/** \file RootPlotFrame.h
    \brief Interface for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootPlotFrame_h
#define st_graph_RootPlotFrame_h

#include <list>
#include <string>
#include <vector>

#include "st_graph/RootFrame.h"

class TAxis;
class TGraph;
class TH2D;
class TMultiGraph;
class TRootEmbeddedCanvas;

namespace st_graph {

  class IFrame;
  class IPlot;
  class ISequence;
  class RootPlot;

  /** \class RootPlotFrame
      \brief A Root frame which is suitable for displaying plots.
  */
  class RootPlotFrame : public RootFrame {
    public:
      /** \brief Construct a frame connected to the given parent, with the given properties.
          \param parent The parent frame.
          \param title The title to display on the frame.
          \param width The width of the frame in pixels.
          \param height The height of the frame in pixels.
          \param delete_parent Flag indicating frame owns (and should delete) parent.
      */
      RootPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height,
        bool delete_parent = false);

      /// \brief Destruct the frame.
      virtual ~RootPlotFrame();

      /// \brief Display this frame and all it contains.
      virtual void display();

      /// \brief Hide this frame and all it contains.
      virtual void unDisplay();

      /** \brief Add the given plot to the frame.
          \param plot The plot to add.
      */
      virtual void addPlot(IPlot * plot);

      /** \brief Remove the given plot from the frame.
          \param plot The plot to remove. If not already in the frame, no harm done.
      */
      virtual void removePlot(IPlot * plot);

      /** \brief Get the title of the frame.
      */
      const std::string & getTitle() const;

    protected:
      /** \brief Internal helper method which correctly displays 2d plots.
          \param axes (Output) set of Root axis objects. Note that axes contains 3 such TAxis objects.
      */
      virtual void display2d(std::vector<TAxis *> & axes);

      /** \brief Internal helper method which correctly displays 3d plots.
          \param axes (Output) set of Root axis objects.
      */
      virtual void display3d(std::vector<TAxis *> & axes);

      /** \brief Internal helper method which creates histogram plot as a Root object.
          \param x The first dimension.
          \param y The second dimension.
      */
      virtual TGraph * createHistPlot(const ISequence & x, const ISequence & y);

      /** \brief Internal helper method which creates scatter plot as a Root object.
          \param x The first dimension.
          \param y The second dimension.
      */
      virtual TGraph * createScatterPlot(const ISequence & x, const ISequence & y);

      /** \brief Internal helper method which creates 2d plot as a Root object.
          \param root_name The name given to the created Root object. Should be unique to avoid warnings from Root.
          \param x The first dimension.
          \param y The second dimension.
	  \param z The third dimension.
      */
      virtual TH2D * createHistPlot2D(const std::string & root_name, const ISequence & x, const ISequence & y,
        const std::vector<std::vector<double> > & z);

      /** \brief Internal helper method which creates a name for Root objects from the given prefix and a pointer.
          \param prefix String prefix for the Root object.
	  \param ptr A pointer which will be concatenated with the prefix to form the name.
      */
      virtual std::string createRootName(const std::string & prefix, void * ptr) const;

      /// \brief Get the underlying Root graphical object; create it if it does not yet exist.
      virtual TMultiGraph * getMultiGraph();

    private:
      std::list<RootPlot *> m_plots;
      std::list<TGraph *> m_tgraphs;
      std::string m_title;
      TRootEmbeddedCanvas * m_canvas;
      TMultiGraph * m_multi_graph;
      TH2D * m_th2d;
      unsigned int m_dimensionality;
  };

}

#endif
