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
#include "st_graph/IEventReceiver.h"
#include "st_graph/IFrame.h"
#include "st_graph/IPlot.h"
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

    /// \brief Test GUI widgets.
    virtual void testGuis();

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
  testGuis();

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

  // Create local reference to engine singleton. This engine is an abstract factory for graphical objects.
  Engine * engine_p = 0;
  try {
    engine_p = &(Engine::instance());
  } catch (const std::exception & x) {
    std::cerr << "Exception while creating engine: " << x.what() << std::endl;
    std::cerr << "WARNING: Test Aborted!" << std::endl;
    return;
  }

  Engine & engine(*engine_p);

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
  IFrame * mf = engine.createMainFrame(0, 600, 400);

  // Create a new subframe in which to display the plots.
  IFrame * pf1 = engine.createPlotFrame(mf, 600, 400);
  
  // Create a scatter plot of this data set, in the subframe.
  IPlot * plot1 = engine.createPlot(pf1, "Scatter", "Quadratic", ValueSet(x1, delta_x1), ValueSet(y1, delta_y1));

  // Modify the data set; double the spreads and shift the plot down.
  for (int ii = 0; ii < num_pts; ++ii) {
    delta_x1[ii] = .4;
    y1[ii] = .3 * (140. - (ii + .3) * (ii + .3));
    delta_y1[ii] = sqrt(fabs(y1[ii]));
  }

  delta_x1[num_pts / 2] = .6;
  delta_x1[num_pts / 2 + 1] = 1.4;

  // Create a histogram plot of this data set, in the subframe.
  IPlot * plot2 = engine.createPlot(pf1, "hist", "Quadratic", ValueSet(x1, delta_x1), ValueSet(y1, delta_y1));

  // Run the graphics engine to display everything.
  engine.run();

  // Clean up.
  delete plot2; plot2 = 0;
  delete plot1; plot1 = 0;
  delete pf1; pf1 = 0;

  // Create rectangular 2d data.
  std::vector<std::vector<double> > hist(2 * num_pts, std::vector<double>(num_pts));
  double x0 = -.5 * num_pts;
  double y0 = 9. - num_pts / 3.;
  for (int ii = 0; ii < num_pts * 2; ++ii) {
    for (int jj = 0; jj < num_pts; ++jj) {
      double x = .5 * (ii - num_pts);
      double y = (jj - num_pts/3.);
      hist[ii][jj] = exp(-(x * x + y * y) / (2. * num_pts)) - exp(-(x0 * x0 + y0 * y0) / (2. * num_pts));
    }
  }
  
  // Create bin definition for second dimension.
  std::vector<double> x2(num_pts * 2);
  std::vector<double> delta_x2(num_pts * 2);
  for (int ii = 0; ii < num_pts * 2; ++ii) {
    x2[ii] = ii;
    delta_x2[ii] = .2;
  }

  // Create a new subframe in which to display new plots.
  pf1 = engine.createPlotFrame(mf, 600, 400);
  
  // Plot the data as a histogram, using previous 1D bin defs for second dimension, new defs for first.
  plot1 = engine.createPlot(pf1, "surf", "2D Gaussian", ValueSet(x2, delta_x2), ValueSet(x1, delta_x1), hist);

  // Run the graphics engine to display everything.
  engine.run();

  // Clean up.
  delete plot2;
  delete plot1;
  delete pf1;
  delete mf;
}

void StGraphTestApp::testGuis() {
  using namespace st_graph;

  class MyGui : public IEventReceiver {
    public:
      MyGui(Engine & engine): m_engine(engine), m_main_frame(0), m_cancel_button(0), m_ok_button(0) {
        // Create a top level main frame in which to place graphical objects.
        m_main_frame = m_engine.createMainFrame(this, 600, 400);

        // Create a couple test buttons.
        m_cancel_button = m_engine.createButton(m_main_frame, this, "text", "Cancel");
        m_ok_button = m_engine.createButton(m_main_frame, this, "text", "OK");
      }

      virtual ~MyGui() { delete m_ok_button; delete m_cancel_button; delete m_main_frame; }

      virtual void run() {
        m_engine.run();
      }

      virtual void clicked(IFrame * f) {
        if (f == m_cancel_button) {
          m_engine.stop();
        } else if (f == m_ok_button) std::cout << "OK button was clicked" << std::endl;
        else std::cout << "Something unknown clicked" << std::endl;
      }

      virtual void closeWindow(IFrame * f) {
        if (f == m_main_frame) {
          m_engine.stop();
        }
      }

    private:
      Engine & m_engine;
      IFrame * m_main_frame;
      IFrame * m_cancel_button;
      IFrame * m_ok_button;
  };

  try {
    MyGui gui(Engine::instance());

    gui.run();
  } catch (const std::exception & x) {
    std::cerr << "Exception while creating engine: " << x.what() << std::endl;
    std::cerr << "WARNING: Test Aborted!" << std::endl;
    return;
  }
}

void StGraphTestApp::reportUnexpected(const std::string & text) const {
  m_failed = true;
  std::cerr << "Unexpected: " << text << std::endl;
}

st_app::StAppFactory<StGraphTestApp> g_factory;
