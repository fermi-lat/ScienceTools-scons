/** \file Engine.h
    \brief Interface which encapsulates a particular graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_Engine_h
#define st_graph_Engine_h

#include <string>
#include <vector>

namespace st_graph {

  class IEventReceiver;
  class IFrame;
  class IPlot;
  class ISequence;

  /** \class Engine
      \brief Interface which encapsulates a particular graphics implementation. This singleton has two purposes:
      1. It is an abstract factory for graphical objects, and 2. It has a run() method which causes the graphical
      engine to execute.
  */
  class Engine {
    public:
      /// \brief Return the singleton (Root) graphics interface.
      static Engine & instance();

      virtual ~Engine();

      /// \brief Run the graphics engine, displaying all graphical objects currently constructed.
      virtual void run() = 0;

      /// \brief Stop the graphics engine, undisplaying all graphical objects currently constructed.
      virtual void stop() = 0;

      /** \brief Create a self-contained two dimensional plot window.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param style The type of plot, e.g. hist, scat.
          \param x The first dimension being plotted, giving the bin definitions.
          \param y The second dimension being plotted, giving the bin values.
      */
      virtual IFrame * createPlot(const std::string & title, unsigned int width, unsigned int height, const std::string & style,
        const ISequence & x, const ISequence & y) = 0;

      /** \brief Create a self-contained three dimensional plot window.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param style The type of plot, e.g. hist, scat.
          \param x The first dimension being plotted, giving the x bin definitions.
          \param y The second dimension being plotted, giving the y bin definitions.
          \param z The third dimension being plotted.
      */
      virtual IFrame * createPlot(const std::string & title, unsigned int width, unsigned int height, const std::string & style,
        const ISequence & x, const ISequence & y, const std::vector<std::vector<double> > & z) = 0;

      /** \brief Create a top-level frame on the desktop. This is the first window which should be created.
          \param receiver The receiver of GUI signals.
          \param width The width of the plot window.
          \param height The height of the plot window.
      */
      virtual IFrame * createMainFrame(IEventReceiver * receiver, unsigned int width, unsigned int height) = 0;

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed.
          \param style The plot style: currently hist* or scat* will be recognized, case insensitive, to mean
                 histogram or scatter plot, respectively.
          \param x The first dimension being plotted.
          \param x The second dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y) = 0;

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed.
          \param style The plot style:
          \param x The first dimension being plotted.
          \param y The second dimension being plotted.
          \param z The third dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y,
        const std::vector<std::vector<double> > & z) = 0;

      /** \brief Create a frame specifically devoted to holding plots. This method will be removed shortly,
                 because a new technique is planned to support plots within any frame.
          \param parent The frame in which to embed the plot frame.
          \param title The title of the plot.
          \param width The width of the frame in pixels.
          \param height The height of the frame in pixels.
      */
      virtual IFrame * createPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height) = 0;

      virtual IFrame * createButton(IFrame * parent, IEventReceiver * receiver, const std::string & style,
        const std::string & text) = 0;

    protected:
      /// \brief Create an engine.
      Engine();

    private:
      /// \brief Copy-construction is not allowed.
      Engine(const Engine &);
  };
}

#endif
