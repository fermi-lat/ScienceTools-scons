/** \file RootEngine.h
    \brief Declaration for class which encapsulates the Root graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootEngine_h
#define st_graph_RootEngine_h

#include <list>
#include <string>
#include <vector>

#include "st_graph/Engine.h"
#include "st_graph/PlotHist.h"

namespace st_graph {

  class IFrame;
  class ISequence;

  /** \class RootEngine
      \brief Declaration for class which encapsulates the Root graphics implementation.
  */
  class RootEngine : public Engine {
    public:
      /// \brief Container of plots registered with this engine.
      typedef std::list<IFrame *> FrameList_t;

      /// \brief Create the Root graphics engine. Creates a Root TApplication object.
      RootEngine();

      /** \brief Run the graphics engine, displaying all graphical objects currently constructed.
          This runs the Root TApplication object to handle window event loop.
      */
      virtual void run();

      /// \brief Stop the graphics engine, undisplaying all graphical objects currently constructed.
      virtual void stop();

      /** \brief Create a top-level frame on the desktop. This is the first window which should be created.
          \param receiver The receiver of GUI signals.
          \param width The width of the plot window.
          \param height The height of the plot window.
      */
      virtual IFrame * createMainFrame(IEventReceiver * receiver, unsigned int width, unsigned int height);

      /** \brief Create a plotter for a one dimensional histogram.
          \param title The title of the plot.
          \param width The width of the plot frame.
          \param height The height of the plot frame.
          \param intervals Set of intervals of the histogram, used to set the plot axes.
      */
      virtual PlotHist * createPlotHist1D(const std::string & title, unsigned int width, unsigned int height,
        const PlotHist::IntervalCont_t & intervals);

      /** \brief Create a plotter for a two dimensional histogram.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param x_intervals Set of intervals of the histogram, used to set the plot X axis.
          \param y_intervals Set of intervals of the histogram, used to set the plot Y axis.
      */
      virtual PlotHist * createPlotHist2D(const std::string & title, unsigned int width, unsigned int height,
        const PlotHist::IntervalCont_t & x_intervals, const PlotHist::IntervalCont_t & y_intervals);

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed.
          \param style The plot style: currently hist* or scat* will be recognized, case insensitive, to mean
                 histogram or scatter plot, respectively.
          \param x The first dimension being plotted.
          \param y The first dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y);

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed.
          \param style The plot style:
          \param x The first dimension being plotted.
          \param y The second dimension being plotted.
          \param z The third dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y,
        const std::vector<std::vector<double> > & z);

      /** \brief Create a frame specifically devoted to holding plots. This method will be removed shortly,
                 because a new technique is planned to support plots within any frame.
          \param parent The frame in which to embed the plot frame.
          \param title The title of the plot.
          \param width The width of the frame in pixels.
          \param height The height of the frame in pixels.
      */
      virtual IFrame * createPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height);

      virtual IFrame * createButton(IFrame * parent, IEventReceiver * receiver, const std::string & style,
        const std::string & label);

      /** \brief Register a frame with the engine. This allows the engine to display or hide
          its associated graphical frames (Root-implementation-specific)
          \param frame The graphical frame to register.
      */
      virtual void addFrame(IFrame * frame);

      /** \brief Unregister a frame with the engine. Following a call to this, the given frame will
          not be displayed or hidden when the engine runs. (Root-implementation-specific)
          \param frame The graphical frame to unregister.
      */
      virtual void removeFrame(IFrame * frame);

      /// \brief Hide all frames from view.
      virtual void hideFrames();

      /// \brief Return whether graphics engine was successfully initialized.
      virtual bool initSucceeded() const { return m_init_succeeded; }

    private:
      FrameList_t m_frames;
      bool m_init_succeeded;
  };

}

#endif
