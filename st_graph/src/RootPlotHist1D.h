/** \file RootPlotHist1D.cxx
    \brief Interface for Root plotter for 1D histograms.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootPlotHist1D
#define st_graph_RootPlotHist1D

#include <string>

#include "st_graph/PlotHist1D.h"

class TH1D;
class TRootEmbeddedCanvas;

namespace st_graph {

  class RootEngine;
  class STGMainFrame;

  /** \class RootPlotHist1D
      \brief Root plotter for 1D histograms.
  */
  class RootPlotHist1D : public PlotHist1D {
    public:
      /** \brief Construct a plotter.
          \param engine The top level application/engine object which controls this plot.
          \param title The title to display on the plot
          \param width The width of the plot window, in pixels
          \param height The height of the plot window, in pixels
          \param intervals The interval definitions of the histogram to be displayed.
      */
      RootPlotHist1D(RootEngine * engine, const std::string & title, unsigned int width, unsigned int height,
        const PlotHist1D::IntervalCont_t & intervals);

      /// \brief Destruct a plot.
      virtual ~RootPlotHist1D();

      /// \brief Display this plot.
      virtual void display();

      /// \brief Hide this plot.
      virtual void unDisplay();

      /** \brief Set the given bin in the plot to have the given value.
          \param index The index of the plot bin.
          \param value The value to plot for that bin.
      */
      virtual void set(int index, double value);

    private:
      RootEngine * m_engine;
      double * m_root_intervals;
      STGMainFrame * m_main_frame;
      TRootEmbeddedCanvas * m_canvas;
      TH1D * m_hist;
      unsigned int m_num_root_intervals;
  };

}

#endif
