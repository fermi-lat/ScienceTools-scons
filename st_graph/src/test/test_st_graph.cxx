/** \file test_st_graph.cxx
    \brief Test code for plotting/graphics
    \author James Peachey, HEASARC/GSSC
*/
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef WIN32
// For sleep.
#include <unistd.h>
#endif

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_graph/Engine.h"
#include "st_graph/IFrame.h"
#include "st_graph/PlotHist.h"
#include "st_graph/ValueSet.h"

/** \class StGraphTestApp
    \brief Test application class.
*/
class StGraphTestApp : public st_app::StApp {
  public:
    /// \brief Perform all tests.
    virtual void run();

    /// \brief Test the ValueSet class.
    virtual void testValueSet();

    /// \brief Test scatter plots.
    virtual void testPlots();

    /// \brief Report failed tests, and set a flag used to exit with non-0 status if an error occurs.
    void reportUnexpected(const std::string & text) const;

  private:
    static bool m_failed;
};

bool StGraphTestApp::m_failed = false;

void StGraphTestApp::run() {
  using namespace st_graph;

  testValueSet();
  testPlots();

  // Test will involve plotting histograms with 200 intervals.
  int num_intervals = 200;

  // Create a set of interval definitions to use for the histogram plot. Make them equal linear bins.
  PlotHist::IntervalCont_t intervals(num_intervals);
  for (int ii = 0; ii < num_intervals; ++ii) intervals[ii] = PlotHist::Interval_t(ii, ii + 1);

  // Create an array containing a sinusoid with 25 extra points at the end to allow it to look like a cosine too.
  const double pi = asin(1) * 2.;
  std::vector<double> sine_wave(num_intervals + 25);

  // Two cycles of this sine will fit nicely in a plot with 200 points.
  for (int ii = 0; ii < num_intervals + 25; ++ii) sine_wave[ii] = sin(ii * 2 * pi / 100.);

  // Create local reference to engine singleton. This engine is an abstract factory for graphical objects.
  Engine * engine = 0;
  try {
    engine = &(Engine::instance());
  } catch (const std::exception & x) {
    std::cerr << "Exception while creating engine: " << x.what() << std::endl;
    std::cerr << "WARNING: Test Aborted!" << std::endl;
    return;
  }

  // Create a histogram plot, size 900 x 600, with the given bin definitions.
  PlotHist * plot_hist_1 = engine->createPlotHist1D("Plot 1", 900, 600, intervals);

  // Create a histogram plot, size 600 x 400, with the (same) given bin definitions.
  PlotHist * plot_hist_2 = engine->createPlotHist1D("Plot 2", 600, 400, intervals);

  // Fill plots with sinusoids.
  for (int ii = 0; ii < num_intervals; ++ii) {
    plot_hist_1->set(ii, sine_wave[ii]);
    plot_hist_2->set(ii, sine_wave[ii + 25]); // cosine
  }

  // Create a 2-d histogram plot, populate it with a 2-d Gaussian.
  PlotHist * plot_hist_3 = engine->createPlotHist2D("Plot 3", 600, 400, intervals, intervals);

  double sigma_squared = 2. * num_intervals;
  for (int ii = 0; ii < num_intervals; ++ii) {
    double x_squared = (ii - num_intervals / 2.) * (ii - num_intervals / 2.);
    for (int jj = 0; jj < num_intervals; ++jj) {
      double y_squared = (jj - num_intervals / 2.) * (jj - num_intervals / 2.);
      plot_hist_3->set(ii, jj, 100. * exp(-(x_squared + y_squared) / (2. * sigma_squared)));
    }
  }

  // Display all graphical objects.
  engine->run();

#ifndef WIN32
  sleep(1); // All windows should disappear briefly.
#endif

  // Reverse the plots , so sine -> cosine and vice versa on the two plots.
  for (int ii = 0; ii < num_intervals; ++ii) {
    plot_hist_1->set(ii, sine_wave[ii + 25]); // cosine
    plot_hist_2->set(ii, sine_wave[ii]);
  }

  // Display all graphical objects (again).
  engine->run();

  // Remove one plot.
  delete plot_hist_3;

#ifndef WIN32
  sleep(1); // All windows should disappear briefly.
#endif

  // Display all graphical objects (again).
  engine->run();

  // Remove the other plots.
  delete plot_hist_2;

  // Remove the other plot.
  delete plot_hist_1;

#ifdef MAKE_TEST_HANG

#ifndef WIN32
  sleep(1); // All windows should disappear.
#endif

  // Display all graphical objects (in this case because all plots were deleted, this will just hang).
  engine->run();
#endif

  if (m_failed) throw std::runtime_error("Unit test failed");
}

void StGraphTestApp::testValueSet() {
  using namespace st_graph;

  std::vector<double> values1(100, 10.);
  std::vector<double> spreads1(100, 1.);
  std::vector<double> values2(200, 20.);
  std::vector<double> spreads2(200, 2.);

  ValueSet v1(values1, spreads1);
  if (values1 != v1.getValues()) reportUnexpected("testValueSet() found values were not the same after creating v1");
  if (spreads1 != v1.getSpreads()) reportUnexpected("testValueSet() found spreads were not the same after creating v1");

  ValueSet v2(values2, spreads1);
  if (values2 != v2.getValues()) reportUnexpected("testValueSet() found values were not the same after creating v2");

  const std::vector<double> & spreads(v2.getSpreads());
  for (unsigned int ii = 0; ii < spreads1.size(); ++ii) {
    if (spreads[ii] != spreads1[ii]) {
      reportUnexpected("testValueSet() found value of spread which disagreed with original value");
      break;
    }
  }

  for (unsigned int ii = spreads1.size(); ii < spreads.size(); ++ii) {
    if (0. != spreads[ii]) {
      reportUnexpected("testValueSet() found padded spread value which was not 0.");
      break;
    }
  }
}

void StGraphTestApp::testPlots() {
  using namespace st_graph;

  Engine & engine(Engine::instance());

  // Set up some fake data.
  int num_pts = 10;
  std::vector<double> x1(num_pts);
  std::vector<double> delta_x1(num_pts);
  std::vector<double> y1(num_pts);
  std::vector<double> delta_y1(num_pts);
  for (int ii = 0; ii < num_pts; ++ii) {
    x1[ii] = ii;
    delta_x1[ii] = .2;
    y1[ii] = .3 * (180. - (ii - .1) * (ii - .1));
    delta_y1[ii] = sqrt(fabs(y1[ii]));
  }

  // Create a top level main frame in which to place graphical objects.
  IFrame * mf = engine.createMainFrame(600, 400);

  // Create a new subframe in which to display the plots.
  IFrame * pf1 = engine.createPlotFrame(mf, 600, 400);
  
  // Create a scatter plot of this data set, in the subframe.
  IFrame * plot1 = engine.createPlot(pf1, "Scatter", "Quadratic", ValueSet(x1, delta_x1), ValueSet(y1, delta_y1));

  // Modify the data set; double the spreads and shift the plot down.
  for (int ii = 0; ii < num_pts; ++ii) {
    x1[ii] = ii;
    delta_x1[ii] = .4;
    y1[ii] = .3 * (140. - (ii + .3) * (ii + .3));
    delta_y1[ii] = sqrt(fabs(y1[ii]));
  }

  delta_x1[num_pts / 2] = .6;
  delta_x1[num_pts / 2 + 1] = 1.4;

  // Create a histogram plot of this data set, in the subframe.
  IFrame * plot2 = engine.createPlot(pf1, "hist", "Quadratic", ValueSet(x1, delta_x1), ValueSet(y1, delta_y1));

  // Run the graphics engine to display everything.
  engine.run();

  // Clean up.
  delete plot2;
  delete plot1;
  delete pf1;
  delete mf;
}

void StGraphTestApp::reportUnexpected(const std::string & text) const {
  m_failed = true;
  std::cerr << "Unexpected: " << text << std::endl;
}

st_app::StAppFactory<StGraphTestApp> g_factory;
