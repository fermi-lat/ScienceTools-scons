/** \file Engine.h
    \brief Interface which encapsulates a particular graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_Engine_h
#define st_graph_Engine_h

#include <string>
#include <vector>

#include "st_graph/PlotHist.h"
#include "st_graph/ValueSet.h"

namespace st_graph {

  class IEventReceiver;
  class IFrame;
  class IPlot;

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

      /** \brief Create a top-level frame on the desktop. This is the first window which should be created.
          \param receiver The receiver of GUI signals.
          \param width The width of the plot window.
          \param height The height of the plot window.
      */
      virtual IFrame * createMainFrame(IEventReceiver * receiver, unsigned int width, unsigned int height) = 0;

      /** \brief Create a plotter for a one dimensional histogram. This method will be removed soon in favor
                 of the more generic createPlot method.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param intervals Set of intervals of the histogram, used to set the plot axes.
      */
      virtual PlotHist * createPlotHist1D(const std::string & title, unsigned int width, unsigned int height,
        const PlotHist::IntervalCont_t & intervals) = 0;

      /** \brief Create a plotter for a two dimensional histogram.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param x_intervals Set of intervals of the histogram, used to set the plot X axis.
          \param y_intervals Set of intervals of the histogram, used to set the plot Y axis.
      */
      virtual PlotHist * createPlotHist2D(const std::string & title, unsigned int width, unsigned int height,
        const PlotHist::IntervalCont_t & x_intervals, const PlotHist::IntervalCont_t & y_intervals) = 0;

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed.
          \param style The plot style: currently hist* or scat* will be recognized, case insensitive, to mean
                 histogram or scatter plot, respectively. Note that for histogram plots, the "spreads" vector
                 from ValueSet is interpreted as the full bin widths. However, for scatter plots, the "spreads"
                 vector represents the absolute size of the symmetric errror (+/-). Thus, for the same ValueSet,
                 a scatter plot will have total spread (max - min) twice as large as for a histogram plot.
          \param x The first dimension being plotted.
          \param y The second dimension being plotted.
          \param z The third dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * parent, const std::string & style, const std::string & title, const ValueSet & x,
        const ValueSet & y, const ValueSet & z = ValueSet()) = 0;

      /** \brief Create a plot which may be displayed in a plot frame.
          \param parent The parent frame in which the plot will be displayed.
          \param style The plot style:
          \param x The first dimension being plotted.
          \param y The second dimension being plotted.
          \param z The third dimension being plotted.
      */
      virtual IPlot * createPlot(IFrame * parent, const std::string & style, const std::string & title, const ValueSet & x,
        const ValueSet & y, const std::vector<std::vector<double> > & z) = 0;

      /** \brief Create a frame specifically devoted to holding plots. This method will be removed shortly,
                 because a new technique is planned to support plots within any frame.
          \param parent The frame in which to embed the plot frame.
          \param width The width of the frame in pixels.
          \param height The height of the frame in pixels.
      */
      virtual IFrame * createPlotFrame(IFrame * parent, unsigned int width, unsigned int height) = 0;

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
