/** \file RootEngine.h
    \brief Declaration for class which encapsulates the Root graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootEngine_h
#define st_graph_RootEngine_h

#include <list>
#include <string>

#include "st_graph/Engine.h"

namespace st_graph {

  class PlotHist1D;

  /** \class RootEngine
      \brief Declaration for class which encapsulates the Root graphics implementation.
  */
  class RootEngine : public Engine {
    public:
      /// \brief Container of plots registered with this engine.
      typedef std::list<PlotHist1D *> FrameList_t;

      /// \brief Create the Root graphics engine. Creates a Root TApplication object.
      RootEngine();

      /** \brief Run the graphics engine, displaying all graphical objects currently constructed.
          This runs the Root TApplication object to handle window event loop.
      */
      virtual void run();

      /** \brief Create a plotter for a one dimensional histogram.
          \param title The title of the plot.
          \param width The width of the plot frame.
          \param height The height of the plot frame.
          \param intervals Set of intervals of the histogram, used to set the plot axes.
      */
      virtual PlotHist1D * createPlotHist1D(const std::string & title, unsigned int width, unsigned int height,
        const PlotHist1D::IntervalCont_t & intervals);

      /** \brief Create a plotter for a two dimensional histogram.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param x_intervals Set of intervals of the histogram, used to set the plot X axis.
          \param y_intervals Set of intervals of the histogram, used to set the plot Y axis.
      */
      virtual PlotHist1D * createPlotHist2D(const std::string & title, unsigned int width, unsigned int height,
        const PlotHist1D::IntervalCont_t & x_intervals, const PlotHist1D::IntervalCont_t & y_intervals);

      /** \brief Register a frame with the engine. This allows the engine to display or hide
          its associated graphical frames (Root-implementation-specific)
          \param frame The graphical frame to register.
      */
      virtual void addFrame(PlotHist1D * frame);

      /** \brief Unregister a frame with the engine. Following a call to this, the given frame will
          not be displayed or hidden when the engine runs. (Root-implementation-specific)
          \param frame The graphical frame to unregister.
      */
      virtual void removeFrame(PlotHist1D * frame);

      /// \brief Hide all frames from view.
      virtual void hideFrames();

    private:
      FrameList_t m_frames;
  };

}

#endif
