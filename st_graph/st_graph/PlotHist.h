/** \file PlotHist1D.h
    \brief Interface for base class plotter for 1D histograms.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_PlotHist1D_h
#define st_graph_PlotHist1D_h

#include <string>
#include <utility>
#include <vector>

namespace st_graph {

  /** \class PlotHist1D
      \brief Interface for base class plotter for 1D histograms.
  */
  class PlotHist1D {
    public:
      /// \brief Convenience typedef for bin interval definitions.
      typedef std::pair<double, double> Interval_t;

      /// \brief Convenience typedef for a container of bin interval definitions.
      typedef std::vector<Interval_t> IntervalCont_t;

      /** \brief Create a 1-d histogram plotter.
          \param title The title to display on the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
          \param intervals Set of intervals (bin definitions) of the histogram, used to set the plot axes.
      */
      PlotHist1D(const std::string & title, unsigned int width, unsigned int height, const IntervalCont_t & intervals);

      /// \brief Destruct the plotter.
      virtual ~PlotHist1D();

      /// \brief Display this plot.
      virtual void display() = 0;

      /// \brief Hide this plot.
      virtual void unDisplay() = 0;

      /** \brief Set the given bin in the plot to have the given value.
          \param index The index of the plot bin.
          \param value The value to plot for that bin.
      */
      virtual void set(int index, double value) = 0;

    protected:
      IntervalCont_t m_intervals;
      std::string m_title;
      unsigned int m_width;
      unsigned int m_height;
  };

}

#endif
