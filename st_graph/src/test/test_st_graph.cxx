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

#include "st_graph/Axis.h"
#include "st_graph/Engine.h"
#include "st_graph/IEventReceiver.h"
#include "st_graph/IFrame.h"
#include "st_graph/IPlot.h"
#include "st_graph/Placer.h"
#include "st_graph/Sequence.h"

#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

/** \class StGraphTestApp
    \brief Test application class.
*/
class StGraphTestApp {
  public:
    /// \brief Construct the test application.
    StGraphTestApp(int argc, char ** argv): m_out("test_st_graph", "", 2) {}

    virtual ~StGraphTestApp() throw() {}

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

    /// \brief Return 0 if no errors occurred, 1 otherwise.
    int status() const { return int(m_failed); }

  protected:
    void testSequence(const st_graph::ISequence & iseq, const std::string & test_name, const double * value,
      const double * low, const double * high);

  private:
    static bool m_failed;
    st_stream::StreamFormatter m_out;
};

bool StGraphTestApp::m_failed = false;

void StGraphTestApp::run() {
  using namespace st_graph;

  testGuis();
  testSequence();
  testPlots();

  // Test will involve plotting histograms with 200 intervals.
  int num_intervals = 200;

  // Create a set of interval definitions to use for the histogram plot. Make them equal linear bins.
  std::vector<double> intervals(num_intervals);
  for (int ii = 0; ii < num_intervals; ++ii) intervals[ii] = ii;

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

  // Make a couple typedefs to improve readability.
  typedef LowerBoundSequence<std::vector<double>::iterator> LowerBoundSeq_t;
  typedef PointSequence<std::vector<double>::iterator> PointSeq_t;

  // Create a histogram plot, size 900 x 600, with the given bin definitions.
  IPlot * plot_hist_1 = engine->createPlot("Plot 1", 900, 600, "hist", LowerBoundSeq_t(intervals.begin(), intervals.end()),
    PointSeq_t(sine_wave.begin(), sine_wave.begin() + num_intervals));

  std::vector<Axis> * axes(&plot_hist_1->getAxes());
  (*axes)[0].setTitle("t");
  (*axes)[1].setTitle("sin(t)");

  // Create a scatter plot, size 600 x 400, with the (same) given bin definitions, but displaying a cosine.
  IPlot * plot_hist_2 = engine->createPlot("Plot 2", 600, 400, "scat", PointSeq_t(intervals.begin(), intervals.end()),
    PointSeq_t(sine_wave.end() - num_intervals, sine_wave.end()));

  axes = &plot_hist_2->getAxes();
  (*axes)[0].setTitle("t");
  (*axes)[1].setTitle("cos(t)");

  // Reduce size of 2-d plot.
  num_intervals = 50;

  // Create 2d fake data.
  std::vector<std::vector<double> > data2d(num_intervals, std::vector<double>(num_intervals));

  double sigma_squared = 2. * num_intervals;
  for (int ii = 0; ii < num_intervals; ++ii) {
    double x_squared = (ii - num_intervals / 2.) * (ii - num_intervals / 2.);
    for (int jj = 0; jj < num_intervals; ++jj) {
      double y_squared = (jj - num_intervals / 2.) * (jj - num_intervals / 2.);
      data2d[ii][jj] = 100. * exp(-(x_squared + y_squared) / (2. * sigma_squared));
    }
  }

  // Create a 2-d histogram plot, populate it with a 2-d Gaussian.
  IPlot * plot_hist_3 = engine->createPlot("Plot 3", 600, 400, "hist",
    LowerBoundSeq_t(intervals.begin(), intervals.begin() + num_intervals),
    LowerBoundSeq_t(intervals.begin(), intervals.begin() + num_intervals), data2d);

  axes = &plot_hist_3->getAxes();
  (*axes)[0].setTitle("X");
  (*axes)[1].setTitle("Y");
  (*axes)[2].setTitle("Z");

  // Display all graphical objects.
  engine->run();

#ifndef WIN32
//  sleep(1); // All windows should disappear briefly.
#endif

  // Remove one plot.
  delete plot_hist_3;

#ifndef WIN32
//  sleep(1); // All windows should disappear briefly.
#endif

  // Display all graphical objects (again).
  engine->run();

  // Remove the other plots.
  delete plot_hist_2;

  // Remove the other plot.
  delete plot_hist_1;

#ifdef MAKE_TEST_HANG

#ifndef WIN32
//  sleep(1); // All windows should disappear.
#endif

  // Display all graphical objects (in this case because all plots were deleted, this will just hang).
  engine->run();
#endif

  if (m_failed) throw std::runtime_error("Unit test failed");
}

void StGraphTestApp::testPlots() {
  using namespace st_graph;

  m_out.setMethod("testPlots()");

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

  // Set axis title.
  std::vector<Axis> * axes(&plot1->getAxes());
  (*axes)[0].setTitle("Correct X axis label");

  // Crate a different Y axis which is scaled down from the original Y axis.
  Vec_t y1lower(y1);
  for (int ii = 0; ii < num_pts; ++ii) {
    y1lower[ii] = .3 * (140. - (ii + .3) * (ii + .3));
  }

  // Create a histogram plot of this data set, in the subframe, ignoring errors.
  IPlot * plot2 = engine.createPlot(pf1, "hist", ValueSeq_t(x1.begin(), x1.end()), ValueSeq_t(y1lower.begin(), y1lower.end()));

  // Set axes titles.
  axes = &plot2->getAxes();
  (*axes)[0].setTitle("INCORRECT X axis label");
  (*axes)[1].setTitle("Correct Y axis label");

  // Run the graphics engine to display everything.
  engine.run();

  // Clean up. It would not hurt to delete the plots, but pf1's destructor will delete them automatically,
  // if they were not already deleted.
  // delete plot2; plot2 = 0;
  // delete plot1; plot1 = 0;
  // It *is* necessary to delete pf1, because otherwise this plot would be displayed again below.
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

  try {
    // Attempt to create a second plot, but overlays are not supported for 3d plots.
    engine.createPlot(pf1, "surf", ValueSpreadSeq_t(x2.begin(), x2.end(), delta_x2.begin()),
      ValueSpreadSeq_t(x1.begin(), x1.end(), delta_x1.begin()), hist);
    m_failed = true;
    m_out.err() << "Creating more than one 3D plot did not throw an exception." << std::endl;
  } catch (const std::exception &) {
    // Expected.
  }

  // Set axes titles.
  axes = &plot1->getAxes();
  (*axes)[0].setTitle("Correct X axis");
  (*axes)[1].setTitle("Correct Y axis");
  (*axes)[2].setTitle("Correct Z axis");

  // Run the graphics engine to display everything.
  engine.run();

  // Clean up. It would not hurt to delete objects owned by mf, but mf's destructor will delete pf1 automatically,
  // which in turn will delete the plots if they were not already deleted.
  // delete plot2;
  // delete plot1;
  // delete pf1; pf1 = 0;
  delete mf;
}

void StGraphTestApp::testGuis() {
  using namespace st_graph;

  class MyGui : public IEventReceiver {
    public:
      MyGui(Engine & engine): m_engine(engine), m_main_frame(0), m_cancel_button(0), m_ok_button(0), m_enable_button(0),
        m_plot_frame(0), m_data(100) {
        // Create a top level main frame in which to place graphical objects.
        m_main_frame = m_engine.createMainFrame(this, 600, 400);

        // Create a couple test buttons.
        m_cancel_button = m_engine.createButton(m_main_frame, this, "text", "Cancel");
        m_ok_button = m_engine.createButton(m_main_frame, this, "text", "OK");
        m_enable_button = m_engine.createButton(m_main_frame, this, "check", "enable");

        // Create a frame to hold a plot.
        m_plot_frame = m_engine.createPlotFrame(m_main_frame, "Linear Function", 520, 400);

        // Create a linear test array.
        for (std::vector<double>::size_type idx = 0; idx != m_data.size(); ++idx) m_data[idx] = idx + 100;

        // Create sequence.
        ValueSequence<std::vector<double>::iterator> val_seq(m_data.begin(), m_data.end());

        // Plot sequence against itself.
        m_engine.createPlot(m_plot_frame, "hist", val_seq, val_seq);
      }

      // Not necessary to delete children widgets, because they will be deleted by main frame's destructor.
      virtual ~MyGui() { delete m_main_frame; }

      virtual void run() {
        m_engine.run();
      }

      virtual void clicked(IFrame * f) {
        if (f == m_cancel_button) {
          m_engine.stop();
        } else if (f == m_enable_button) {
          std::cout << "Enable button is " << m_enable_button->getState() << std::endl;
        } else if (f == m_ok_button) {
          std::cout << "OK button was clicked" << std::endl;
        } else {
          std::cout << "Something unforeseen clicked" << std::endl;
        }
      }

      virtual void closeWindow(IFrame * f) {
        if (f == m_main_frame) {
          m_engine.stop();
        }
      }

      virtual void layout(IFrame *) {
        // Button justifiers.
        LeftEdge le_main(m_main_frame);
        RightEdge re_main(m_main_frame);
        RightEdge re_ok(m_ok_button);
        RightEdge re_cancel(m_cancel_button);

        // Place buttons in row, ok, cancel, enable.
        LeftEdge(m_ok_button).rightOf(le_main);
        LeftEdge(m_cancel_button).rightOf(re_ok);
        TopEdge(m_enable_button).below(BottomEdge(m_cancel_button));

        // Place plot to right of cancel.
        LeftEdge(m_plot_frame).rightOf(re_cancel, 5);
        RightEdge(m_plot_frame).stretchTo(re_main);

        // Place plot below cancel too.
        TopEdge(m_plot_frame).below(BottomEdge(m_cancel_button), 5);
        BottomEdge(m_plot_frame).stretchTo(BottomEdge(m_main_frame));
      }

    private:
      Engine & m_engine;
      IFrame * m_main_frame;
      IFrame * m_cancel_button;
      IFrame * m_ok_button;
      IFrame * m_enable_button;
      IFrame * m_plot_frame;
      std::vector<double> m_data;
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

int main(int argc, char ** argv) {
  int status = 1;
  try {
    StGraphTestApp app(argc, argv);
    app.run();
    status = app.status();
  } catch (const std::exception & x) {
    std::cerr << x.what() << std::endl;
  }

  return status;
}
