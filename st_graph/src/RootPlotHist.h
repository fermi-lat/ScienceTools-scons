/** \file RootPlotHist.cxx
    \brief Interface for Root plotter for all histograms.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootPlotHist
#define st_graph_RootPlotHist

#include <string>

#include "st_graph/PlotHist.h"

class TH1D;
class TH2D;
class TRootEmbeddedCanvas;

namespace st_graph {

  class RootEngine;
  class STGMainFrame;

  /** \class RootPlotHist
      \brief Root plotter for all histograms.
  */
  class RootPlotHist : public PlotHist {
    public:
      /** \brief Construct a 1D plotter.
          \param engine The top level application/engine object which controls this plot.
          \param title The title to display on the plot
          \param width The width of the plot window, in pixels
          \param height The height of the plot window, in pixels
          \param intervals The interval definitions of the histogram to be displayed.
      */
      RootPlotHist(RootEngine * engine, const std::string & title, unsigned int width, unsigned int height,
        const PlotHist::IntervalCont_t & intervals);

      /** \brief Construct a 2D plotter.
          \param engine The top level application/engine object which controls this plot.
          \param title The title to display on the plot
          \param width The width of the plot window, in pixels
          \param height The height of the plot window, in pixels
          \param x_intervals The interval definitions of the histogram (X axis) to be displayed.
          \param y_intervals The interval definitions of the histogram (Y axis) to be displayed.
      */
      RootPlotHist(RootEngine * engine, const std::string & title, unsigned int width, unsigned int height,
        const PlotHist::IntervalCont_t & x_intervals, const PlotHist::IntervalCont_t & y_intervals);

      /// \brief Destruct a plot.
      virtual ~RootPlotHist();

      /// \brief Display this plot.
      virtual void display();

      /// \brief Hide this plot.
      virtual void unDisplay();

      /** \brief Set the given bin in the plot to have the given value.
          \param index The index of the plot bin.
          \param value The value to plot for that bin.
      */
      virtual void set(int index, double value);

      /** \brief Set the given bin in the plot to have the given value.
          \param x_index The index of the plot X bin.
          \param y_index The index of the plot Y bin.
          \param value The value to plot for that bin.
      */
      virtual void set(int x_index, int y_index, double value);

      /// \brief Return the number of dimensions in this plot.
      virtual int dimensionality() const;

    private:
      void init();
      double * createIntervals(const PlotHist::IntervalCont_t & intervals) const;

      RootEngine * m_engine;
      double * m_x_intervals;
      double * m_y_intervals;
      STGMainFrame * m_main_frame;
      TRootEmbeddedCanvas * m_canvas;
      TH1D * m_hist1;
      TH2D * m_hist2;
  };

}

#endif
