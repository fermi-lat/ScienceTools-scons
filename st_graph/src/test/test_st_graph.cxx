/** \file test_st_graph.cxx
    \brief Test code for plotting/graphics
    \author James Peachey, HEASARC/GSSC
*/
#include <cmath>
#include <vector>

#ifndef WIN32
// For sleep.
#include <unistd.h>
#endif

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_graph/Engine.h"
#include "st_graph/PlotHist1D.h"

/** \class StGraphTestApp
    \brief Test application class.
*/
class StGraphTestApp : public st_app::StApp {
  public:
    /// \brief Perform all tests.
    virtual void run();
};

void StGraphTestApp::run() {
  using namespace st_graph;

  // Test will involve plotting histograms with 200 intervals.
  int num_intervals = 200;

  // Create a set of interval definitions to use for the histogram plot. Make them equal linear bins.
  PlotHist1D::IntervalCont_t intervals(num_intervals);
  for (int ii = 0; ii < num_intervals; ++ii) intervals[ii] = PlotHist1D::Interval_t(ii, ii + 1);

  // Create an array containing a sinusoid with 25 extra points at the end to allow it to look like a cosine too.
  const double pi = asin(1) * 2.;
  std::vector<double> sine_wave(num_intervals + 25);

  // Two cycles of this sine will fit nicely in a plot with 200 points.
  for (int ii = 0; ii < num_intervals + 25; ++ii) sine_wave[ii] = sin(ii * 2 * pi / 100.);

  // Create local reference to engine singleton. This engine is an abstract factory for graphical objects.
  Engine & engine(Engine::instance());

  // Create a histogram plot, size 900 x 600, with the given bin definitions.
  PlotHist1D * plot_hist_1 = engine.createPlotHist1D("Plot 1", 900, 600, intervals);

  // Create a histogram plot, size 600 x 400, with the (same) given bin definitions.
  PlotHist1D * plot_hist_2 = engine.createPlotHist1D("Plot 2", 600, 400, intervals);

  // Fill plots with sinusoids.
  for (int ii = 0; ii < num_intervals; ++ii) {
    plot_hist_1->set(ii, sine_wave[ii]);
    plot_hist_2->set(ii, sine_wave[ii + 25]); // cosine
  }

  // Display all graphical objects.
  engine.run();

#ifndef WIN32
  sleep(1); // All windows should disappear briefly.
#endif

  // Reverse the plots , so sine -> cosine and vice versa on the two plots.
  for (int ii = 0; ii < num_intervals; ++ii) {
    plot_hist_1->set(ii, sine_wave[ii + 25]); // cosine
    plot_hist_2->set(ii, sine_wave[ii]);
  }

  // Display all graphical objects (again).
  engine.run();

  // Remove one plot.
  delete plot_hist_2;

#ifndef WIN32
  sleep(1); // All windows should disappear briefly.
#endif

  // Display all graphical objects (again).
  engine.run();

  // Remove the other plot.
  delete plot_hist_1;

#ifdef MAKE_TEST_HANG

#ifndef WIN32
  sleep(1); // All windows should disappear.
#endif

  // Display all graphical objects (in this case because all plots were deleted, this will just hang).
  engine.run();
#endif

}

st_app::StAppFactory<StGraphTestApp> g_factory;
