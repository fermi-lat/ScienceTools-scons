/** \file Engine.h
    \brief Interface which encapsulates a particular graphics implementation.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_Engine_h
#define st_graph_Engine_h

#include <string>

#include "st_graph/PlotHist1D.h"

namespace st_graph {

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

      /** \brief Create a plotter for a one dimensional histogram.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param intervals Set of intervals of the histogram, used to set the plot axes.
      */
      virtual PlotHist1D * createPlotHist1D(const std::string & title, unsigned int width, unsigned int height,
        const PlotHist1D::IntervalCont_t & intervals) = 0;

      /** \brief Create a plotter for a two dimensional histogram.
          \param title The title of the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param x_intervals Set of intervals of the histogram, used to set the plot X axis.
          \param y_intervals Set of intervals of the histogram, used to set the plot Y axis.
      */
      virtual PlotHist1D * createPlotHist2D(const std::string & title, unsigned int width, unsigned int height,
        const PlotHist1D::IntervalCont_t & x_intervals, const PlotHist1D::IntervalCont_t & y_intervals) = 0;

    protected:
      /// \brief The singleton engine.
      static Engine * m_engine;

      /// \brief Create an engine.
      Engine();

    private:
      /// \brief Copy-construction is not allowed.
      Engine(const Engine &);
  };
}

#endif
