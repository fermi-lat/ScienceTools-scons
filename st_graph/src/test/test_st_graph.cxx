/** \file test_st_graph.cxx
    \brief Test code for plotting/graphics
    \author James Peachey, HEASARC/GSSC
*/
#include <iostream>
#include <list>
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
#include "st_graph/Sequence.h"

#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

/** \class StGraphTestApp
    \brief Test application class.
*/
class StGraphTestApp : public st_app::StApp {
  public:
    /// \brief Construct the test application.
    StGraphTestApp(): m_out("test_st_graph", "", 2) {}

    /// \brief Perform all tests.
    virtual void run();

    /// \brief Test scatter plots.
    virtual void testPlots();

    /// \brief Test GUI widgets.
    virtual void testGuis();

    /// \brief Test the Sequence template class.
    virtual void testSequence();

    /// \brief Report failed tests, and set a flag used to exit with non-0 status if an error occurs.
    void reportUnexpected(const std::string & text) const;

  protected:
    void StGraphTestApp::testSequence(const st_graph::ISequence & iseq, const std::string & test_name, const double * value,
      const double * low, const double * high);

  private:
    static bool m_failed;
    st_stream::StreamFormatter m_out;
};

bool StGraphTestApp::m_failed = false;

void StGraphTestApp::run() {
  using namespace st_graph;

  testSequence();
  testPlots();
  testGuis();

  // Test will involve plotting histograms with 200 intervals.
  int num_intervals = 200;

  // Create a set of interval definitions to use for the histogram plot. Make them equal linear bins.
  PlotHist::IntervalCont_t intervals(num_intervals);
  for (int ii = 0; ii < num_intervals; ++ii) intervals[ii] = PlotHist::Interval_t(ii, ii + 1);

  // Create an array containing a sinusoid with 25 extra points at the end to allow it to look like a cosine too.
  std::vector<double> sine_wave(num_intervals + 25);

  // Two cycles of this sine will fit nicely in a plot with 200 points.
  for (int ii = 0; ii < num_intervals + 25; ++ii) sine_wave[ii] = sin(ii * 2 * M_PI / 100.);

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

  typedef std::vector<double> Vec_t;
  typedef ValueSequence<Vec_t::iterator> ValueSeq_t;
  typedef ValueSpreadSequence<Vec_t::iterator> ValueSpreadSeq_t;

  // Set up some fake data.
  int num_pts = 10;
  Vec_t x1(num_pts);
  Vec_t delta_x1(num_pts);
  Vec_t y1(num_pts);
  Vec_t delta_y1(num_pts);
  for (int ii = 0; ii < num_pts; ++ii) {
    x1[ii] = ii;
    delta_x1[ii] = .6;
    y1[ii] = .3 * (180. - (ii - .1) * (ii - .1));
    delta_y1[ii] = .2 * fabs(y1[ii]);
  }

  // Create a top level main frame in which to place graphical objects.
  IFrame * mf = engine.createMainFrame(0, 600, 400);

  // Create a new subframe in which to display the plots.
  IFrame * pf1 = engine.createPlotFrame(mf, "Quadratic", 600, 400);

  // Create a scatter plot of this data set, in the subframe.
  IPlot * plot1 = engine.createPlot(pf1, "Scatter", ValueSpreadSeq_t(x1.begin(), x1.end(), delta_x1.begin()),
    ValueSpreadSeq_t(y1.begin(), y1.end(), delta_y1.begin()));

  // Modify the data set by shifting the plot down.
  for (int ii = 0; ii < num_pts; ++ii) {
    y1[ii] = .3 * (140. - (ii + .3) * (ii + .3));
  }

  // Create a histogram plot of this data set, in the subframe, ignoring errors.
  IPlot * plot2 = engine.createPlot(pf1, "hist", ValueSeq_t(x1.begin(), x1.end()), ValueSeq_t(y1.begin(), y1.end()));

  // Run the graphics engine to display everything.
  engine.run();

  // Clean up.
  delete plot2; plot2 = 0;
  delete plot1; plot1 = 0;
  delete pf1; pf1 = 0;

  // Create rectangular 2d data.
  std::vector<Vec_t> hist(2 * num_pts, Vec_t(num_pts));
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
  Vec_t x2(num_pts * 2);
  Vec_t delta_x2(num_pts * 2);
  for (int ii = 0; ii < num_pts * 2; ++ii) {
    x2[ii] = ii;
    delta_x2[ii] = .2;
  }

  // Create a new subframe in which to display new plots.
  pf1 = engine.createPlotFrame(mf, "2D Gaussian", 600, 400);

  // Plot the data as a histogram, using previous 1D bin defs for second dimension, new defs for first.
  plot1 = engine.createPlot(pf1, "surf", ValueSpreadSeq_t(x2.begin(), x2.end(), delta_x2.begin()),
    ValueSpreadSeq_t(x1.begin(), x1.end(), delta_x1.begin()), hist);

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

void StGraphTestApp::testSequence() {
  using namespace st_graph;

  // Typedef for brevity.
  typedef std::vector<double> Vec_t;

  // Test PointSequence.
  {
    // Create a sequence of values, and correct derived edges.
    const double value[] = { 10., 12., 15., 17., 19., 20. };

    // Create a sequence to represent the values, using a simple pointer as the iterator.
    PointSequence<const double *> seq(value, value + sizeof(value) / sizeof(double));

    // Perform detailed test of the sequence.
    testSequence(seq, "PointSequence", value, value, value);

  }

  // Test ValueSequence.
  {
    // Create a sequence of values, and correct derived edges.
    const double value[] = { 10., 12., 15., 17., 19., 20. };
    const double low[] = { 9., 11., 13.5, 16., 18., 19.5 };
    const double high[] = { 11., 13.5, 16., 18., 19.5, 20.5 };

    // Create a sequence to represent the values, using a simple pointer as the iterator.
    ValueSequence<const double *> seq(value, value + sizeof(value) / sizeof(double));

    // Perform detailed test of the sequence.
    testSequence(seq, "ValueSequence", value, low, high);

  }

  // Test LowerBoundSequence.
  {
    // Create a new sequence of left edges, and correct derived values.
    const double left[] = { 10., 12., 15., 17., 19., 20. };
    const double value[] = { 11., 13.5, 16., 18., 19.5, 20.5 };
    const double right[] = { 12., 15., 17., 19., 20., 21. };

    // Copy left edges to vector::iterator.
    Vec_t vec(left, left + sizeof(left)/sizeof(double));

    // Create a sequence to represent the values, using a vector::const_iterator.
    LowerBoundSequence<Vec_t::iterator> seq(vec.begin(), vec.end());

    // Perform detailed test of the sequence.
    testSequence(seq, "LowerBoundSequence", value, left, right);
  }

  // Test ValueSpreadSequence.
  {
    // Create a new sequence of points with error bars.
    const double value[] = { 10., 12., 15., 17., 19., 20. };
    const double low_err[] = { .5, 2., 1., 1.5, .5, 1. };
    const double low[] = { 9.5, 10., 14., 15.5, 18.5, 19. };
    const double high_err[] = { 1.5, 1., 1.5, .5, 2., 1. };
    const double high[] = { 11.5, 13., 16.5, 17.5, 21., 21. };

    ISequence::size_type num_rec = sizeof(value) / sizeof(double);

    // Create a sequence to represent the values, using a vector::const_iterator.
    ValueSpreadSequence<const double *> seq(value, value + num_rec, low_err, high_err);

    // Perform detailed test of the sequence.
    testSequence(seq, "ValueSpreadSequence", value, low, high);
  }

  // Test IntervalSequence.
  {
    // Create a new sequence of left edges, and correct derived values.
    const double left[] = { 10., 12., 15., 17., 19., 20. };
    const double value[] = { 11., 13.5, 16., 18., 19.5, 20.5 };
    const double right[] = { 12., 15., 17., 19., 20., 21. };

    typedef std::list<double> List_t;

    // Copy intervals to list, just to test list iteration.
    List_t left_list(left, left + sizeof(left)/sizeof(double));
    List_t right_list(right, right + sizeof(right)/sizeof(double));

    // Create a sequence to represent the values, using a list::const_iterator.
    IntervalSequence<List_t::iterator> seq(left_list.begin(), left_list.end(), right_list.begin());

    // Perform detailed test of the sequence.
    testSequence(seq, "IntervalSequence", value, left, right);
  }

}

void StGraphTestApp::testSequence(const st_graph::ISequence & iseq, const std::string & test_name, const double * value,
  const double * low, const double * high) {
  // Customize stream message prefix.
  m_out.setMethod("testSequence(const ISequence&, ...)");

  // Typedef for brevity.
  typedef std::vector<double> Vec_t;

  // Create vectors to hold sequence's resultant values.
  Vec_t low_vec;
  Vec_t high_vec;
  Vec_t value_vec;
  Vec_t low_err;
  Vec_t high_err;

  // Get intervals defined by the sequence.
  iseq.getIntervals(low_vec, high_vec);

  // Get values defined by the sequence.
  iseq.getValues(value_vec);

  // Get spreads defined by the sequence.
  iseq.getSpreads(low_err, high_err);

  // Confirm that the intervals have the correct values.
  for (Vec_t::size_type index = 0; index != low_vec.size(); ++index) {
    if (value[index] != value_vec[index]) {
      m_failed = true;
      m_out.err() << test_name << ": getIntervals returned value_vec[" << index << "] == " << value_vec[index] <<
        ", not " << value[index] << std::endl;
    }
    if (low[index] != low_vec[index]) {
      m_failed = true;
      m_out.err() << test_name << ": getIntervals returned low_vec[" << index << "] == " << low_vec[index] <<
        ", not " << low[index] << std::endl;
    }
    if (high[index] != high_vec[index]) {
      m_failed = true;
      m_out.err() << test_name << ": getIntervals returned high_vec[" << index << "] == " << high_vec[index] <<
        ", not " << high[index] << std::endl;
    }
    if (high_err[index] != high_vec[index] - value_vec[index]) {
      m_failed = true;
      m_out.err() << test_name << ": getIntervals returned high_err[" << index << "] == " << high_err[index] <<
        ", not " << high_vec[index] - value_vec[index] << std::endl;
    }
    if (low_err[index] != value_vec[index] - low_vec[index]) {
      m_failed = true;
      m_out.err() << test_name << ": getIntervals returned low_err[" << index << "] == " << low_err[index] <<
        ", not " << value_vec[index] - low_vec[index] << std::endl;
    }
  }
}

void StGraphTestApp::reportUnexpected(const std::string & text) const {
  m_failed = true;
  std::cerr << "Unexpected: " << text << std::endl;
}

st_app::StAppFactory<StGraphTestApp> g_factory("test_st_graph");
