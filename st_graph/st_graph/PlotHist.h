/** \file PlotHist.h
    \brief Interface for base class plotter for all histograms.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_PlotHist_h
#define st_graph_PlotHist_h

#include <string>
#include <utility>
#include <vector>

namespace st_graph {

  /** \class PlotHist
      \brief Interface for base class plotter for all histograms.
  */
  class PlotHist {
    public:
      /// \brief Convenience typedef for bin interval definitions.
      typedef std::pair<double, double> Interval_t;

      /// \brief Convenience typedef for a container of bin interval definitions.
      typedef std::vector<Interval_t> IntervalCont_t;

      /** \brief Create a 1D histogram plotter.
          \param title The title to display on the plot.
          \param width The width of the plot window.
          \param height The height of the plot window.
      */
      PlotHist(const std::string & title, unsigned int width, unsigned int height);

      /// \brief Destruct the plotter.
      virtual ~PlotHist();

      /// \brief Display this plot.
      virtual void display() = 0;

      /// \brief Hide this plot.
      virtual void unDisplay() = 0;

      /** \brief Set the given bin in the plot to have the given value. This throws an exception if
          the plotter is not 1D.
          \param index The index of the plot bin.
          \param value The value to plot for that bin.
      */
      virtual void set(int index, double value) = 0;

      /** \brief Set the given bin in the plot to have the given value. This throws an exception if
          the plotter is not 2D.
          \param x_index The index of the plot X bin.
          \param y_index The index of the plot Y bin.
          \param value The value to plot for that bin.
      */
      virtual void set(int x_index, int y_index, double value) = 0;

      /// \brief Return the number of dimensions in this plot.
      virtual int dimensionality() const = 0;

    protected:
      std::string m_title;
      unsigned int m_width;
      unsigned int m_height;
  };

}

#endif
