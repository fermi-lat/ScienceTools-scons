/** \file test_evtbin.cxx
    \brief Event bin test program.
    \author Yasushi Ikebe, GSSC
            James Peachey, HEASARC
*/

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

// Class encapsulating a count map.
#include "evtbin/CountMap.h"
// Class encapsulating a 1 dimensional histogram.
#include "evtbin/Hist1D.h"
// Class encapsulating a 2 dimensional histogram.
#include "evtbin/Hist2D.h"
// Light curve abstractions.
#include "evtbin/LightCurve.h"
// Class encapsulating description of a binner with linear equal size bins.
#include "evtbin/LinearBinner.h"
// Class encapsulating description of a binner with logarithmically equal size bins.
#include "evtbin/LogBinner.h"
// Class encapsulating description of a binner with ordered but otherwise arbitrary bins.
#include "evtbin/OrderedBinner.h"
// Class for binning into Hist objects from tip objects:
#include "evtbin/RecordBinFiller.h"
// Multiple spectra abstractions.
#include "evtbin/MultiSpec.h"
// Single spectrum abstractions.
#include "evtbin/SingleSpec.h"
// Application base class.
#include "st_app/StApp.h"
// Factory used by st_app's standard main to create application object.
#include "st_app/StAppFactory.h"
// Tip File access.
#include "tip/IFileSvc.h"
// Tip Table access.
#include "tip/Table.h"

/** \class EvtBinTest
    \brief Application singleton for evtbin test program.
*/
class EvtBinTest : public st_app::StApp {
  public:
    EvtBinTest();

    virtual ~EvtBinTest() throw() {}

    /** \brief Run all tests.
    */
    virtual void run();

    void testLinearBinner();

    void testLogBinner();

    void testOrderedBinner();

    void testHist1D();

    void testHist2D();

    void testLightCurve();

    void testSingleSpectrum();

    void testMultiSpectra();

    void testCountMap();

  private:
    std::string m_data_dir;
    bool m_failed;
};

EvtBinTest::EvtBinTest() {
  const char * data_dir = getenv("EVTBINROOT");
  if (0 != data_dir) m_data_dir = data_dir;
  m_data_dir += "/data/";
}

void EvtBinTest::run() {
  m_failed = false;

  // Test linear binner:
  testLinearBinner();
  // Test logarithmic binner:
  testLogBinner();
  // Test ordered binner:
  testOrderedBinner();
  // Test one dimensional histogram:
  testHist1D();
  // Test two dimensional histogram:
  testHist2D();
  // Test light curve with no energy binning (using Tip):
  testLightCurve();
  // Test single spectrum with no time binning (using Tip):
  testSingleSpectrum();
  // Test multiple spectra with no time binning (using Tip):
  testMultiSpectra();
  // Test count map (using Tip):
  testCountMap();

  // Report problems, if any.
  if (m_failed) throw std::runtime_error("Unit test failed");
}

void EvtBinTest::testLinearBinner() {
  using namespace evtbin;
  std::string msg;

  // Create a linear binner with bin width == 15. spanning the interval [0, 100):
  LinearBinner binner(0., 100., 15.);

  // Make sure there are 7 bins:
  msg = "LinearBinner::getNumBins()";
  if (7 != binner.getNumBins()) {
    std::cerr << msg << " returned " << binner.getNumBins() << ", not 7" << std::endl;
    m_failed = true;
  }

  // Make sure values are correctly classified:
  msg = "LinearBinner::computeIndex(";
  for (long ii = 0; ii < 7; ++ii) {
    int value = 9 + ii * 15;
    long index = binner.computeIndex(value);
    if (index != ii) {
      std::cerr << msg << value << ") returned " << index << ", not " << ii << std::endl;
      m_failed = true;
    }
  }

  // Left endpoint should be included:
  long index = binner.computeIndex(0);
  if (0 != index) {
    std::cerr << msg << "0) returned " << index << ", not 0" << std::endl;
    m_failed = true;
  }

  // Right endpoint should be excluded:
  index = binner.computeIndex(100);
  if (-1 != index) {
    std::cerr << msg << "100) returned " << index << ", not -1" << std::endl;
    m_failed = true;
  }

  // Left of left endpoint should return index < 0:
  index = binner.computeIndex(-1);
  if (0 <= index) {
    std::cerr << msg << "-1) returned " << index << ", which is >= 0" << std::endl;
    m_failed = true;
  }

  // Right of right endpoint should return index < 0:
  index = binner.computeIndex(101);
  if (0 <= index) {
    std::cerr << msg << "101) returned " << index << ", which is >= 0" << std::endl;
    m_failed = true;
  }

  // Make sure nice symmetric intervals are also handled correctly.
  LinearBinner binner2(0., 100., 10.);

  // Make sure there are 10 bins:
  msg = "LinearBinner::getNumBins()";
  if (10 != binner2.getNumBins()) {
    std::cerr << msg << " returned " << binner.getNumBins() << ", not 10" << std::endl;
    m_failed = true;
  }

}

void EvtBinTest::testLogBinner() {
  using namespace evtbin;
  std::string msg;

  // Create a log binner with 10 bins spanning the interval [1, exp(15.)):
  LogBinner binner(1., exp(15.), 10);

  // Make sure there are really 10 bins:
  msg = "LogBinner::getNumBins()";
  if (10 != binner.getNumBins()) {
    std::cerr << msg << " returned " << binner.getNumBins() << ", not 10" << std::endl;
    m_failed = true;
  }

  // Make sure values are correctly classified:
  msg = "LogBinner::computeIndex(";
  for (long ii = 0; ii < 10; ++ii) {
    double value = .9999999 * exp((ii + 1) * 15. / 10.);
    long index = binner.computeIndex(value);
    if (index != ii) {
      std::cerr << msg << value << ") returned " << index << ", not " << ii << std::endl;
      m_failed = true;
    }
  }

  // Left endpoint should be included:
  long index = binner.computeIndex(1.);
  if (0 != index) {
    std::cerr << msg << "1.) returned " << index << ", not 0" << std::endl;
    m_failed = true;
  }

  // Right endpoint should be excluded:
  index = binner.computeIndex(exp(15.));
  if (-1 != index) {
    std::cerr << msg << "exp(15.) returned " << index << ", not -1" << std::endl;
    m_failed = true;
  }

  // Left of left endpoint should return index < 0:
  index = binner.computeIndex(0.);
  if (0 <= index) {
    std::cerr << msg << "0.) returned " << index << ", which is >= 0" << std::endl;
    m_failed = true;
  }

  // Right of right endpoint should return index < 0:
  index = binner.computeIndex(1.000001 * exp(15.));
  if (0 <= index) {
    std::cerr << msg << "1.000001 * exp(15.)) returned " << index << ", which is >= 0" << std::endl;
    m_failed = true;
  }
}

void EvtBinTest::testOrderedBinner() {
  using namespace evtbin;

  std::string msg = "OrderedBinner::OrderedBinner(...)";

  OrderedBinner::IntervalCont_t intervals;

  // Create intervals with bad ordering within a given interval.
  intervals.push_back(Binner::Interval(0., .1));
  intervals.push_back(Binner::Interval(.15, .25));
  intervals.push_back(Binner::Interval(.25, .24));
  intervals.push_back(Binner::Interval(.30, .45));

  try {
    OrderedBinner binner(intervals);
    std::cerr << msg << " did not throw when given an interval whose beginning value is > ending value" << std::endl;
    m_failed = true;
  } catch (const std::exception & x) {
  }

  intervals.clear();

  // Create intervals with bad ordering between two subsequent intervals.
  intervals.push_back(Binner::Interval(0., .1));
  intervals.push_back(Binner::Interval(.15, .25));
  intervals.push_back(Binner::Interval(.24, .29));
  intervals.push_back(Binner::Interval(.30, .45));

  try {
    OrderedBinner binner(intervals);
    std::cerr << msg << " did not throw when given two sequential intervals which are not in order" << std::endl;
    m_failed = true;
  } catch (const std::exception & x) {
  }

  // Finally, create a legitimate set of intervals.
  intervals.clear();

  intervals.push_back(Binner::Interval(0., .1));
  intervals.push_back(Binner::Interval(.15, .25));
  intervals.push_back(Binner::Interval(.30, .45));
  intervals.push_back(Binner::Interval(.50, .60));
  intervals.push_back(Binner::Interval(.60, .72));

  try {
    OrderedBinner binner(intervals);

    msg = "OrderedBinner::computeIndex(";

    double value;
    long index;

    // Make sure all values are classified correctly.
    // A value less than the first bin.
    value = -.01;
    index = binner.computeIndex(value);
    if (0 <= index) {
      std::cerr << msg << value << ") returned " << index << ", not a negative index" << std::endl;
    }
    
    // A value greater than the last bin.
    value = 1.;
    index = binner.computeIndex(value);
    if (0 <= index) {
      std::cerr << msg << value << ") returned " << index << ", not a negative index" << std::endl;
    }
    
    // A value in the 0th bin.
    value = .05;
    index = binner.computeIndex(value);
    if (0 != index) {
      std::cerr << msg << value << ") returned " << index << ", not 0" << std::endl;
    }
    
    // A value in the 1st bin.
    value = .17;
    index = binner.computeIndex(value);
    if (1 != index) {
      std::cerr << msg << value << ") returned " << index << ", not 1" << std::endl;
    }
    
    // A value in the 2st bin.
    value = .30;
    index = binner.computeIndex(value);
    if (2 != index) {
      std::cerr << msg << value << ") returned " << index << ", not 2" << std::endl;
    }
    
    // A value in the 3rd bin.
    value = .55;
    index = binner.computeIndex(value);
    if (3 != index) {
      std::cerr << msg << value << ") returned " << index << ", not 3" << std::endl;
    }
    
    // A value in the 4th bin.
    value = .60;
    index = binner.computeIndex(value);
    if (4 != index) {
      std::cerr << msg << value << ") returned " << index << ", not 4" << std::endl;
    }
    
    // A value between bins.
    value = .25;
    index = binner.computeIndex(value);
    if (0 <= index) {
      std::cerr << msg << value << ") returned " << index << ", not a negative index" << std::endl;
    }
  
  } catch (const std::exception & x) {
    std::cerr << msg << " threw when given a set of intervals which are legal (i.e. in order)" << std::endl;
    m_failed = true;
  }
}

void EvtBinTest::testHist1D() {
  using namespace evtbin;
  std::string msg = "Hist1D";

  // Create a linear binner with bin width == 15. spanning the interval [0, 100):
  LinearBinner binner(0., 100., 15.);

  // Create a histogram using this binner:
  Hist1D lin_hist(binner);

  // Populate this histogram, starting from right of the right endpoint, going to left of left endpoint:
  for (int ii = 100; ii >= -1; --ii) lin_hist.fillBin(ii);

  // Last bin has 5 fewer values because the interval is not an integer multiple of the bin size.
  // Below it will be checked that each bin has the same number of counts, so pad it out here.
  for (int ii = 0; ii < 5; ++ii) lin_hist.fillBin(97.);

  // Check whether each bin has the right number:
  int bin_num = 0;
  for (Hist1D::ConstIterator itor = lin_hist.begin(); itor != lin_hist.end(); ++itor, ++bin_num) {
    if (15 != *itor) {
      std::cerr << msg << "'s bin number " << bin_num << " has " << *itor << " counts, not 15" << std::endl;
      m_failed = true;
    }
  }
}

void EvtBinTest::testHist2D() {
  using namespace evtbin;
  std::string msg = "Hist2D";

  // Create a linear binner with bin width == 10 spanning the interval [0, 100):
  LinearBinner binner1(0., 100., 10.);

  // Create a log binner with 10 bins spanning the interval [1, exp(10.)):
  LogBinner binner2(1., exp(10.), 10);

  // Create a histogram using these binners:
  Hist2D hist(binner1, binner2);

  // Populate this histogram, starting from right of the right endpoint, going to left of left endpoint:
  for (int ii = 100; ii >= -1; --ii) {
    for (long jj = 0; jj < 10; ++jj) {
      double value = .9999999 * exp(jj + 1);
      hist.fillBin(ii, value);
    }
  }

  // Check whether each bin has the right number:
  int bin_num1 = 0;
  for (Hist2D::ConstIterator1 itor1 = hist.begin(); itor1 != hist.end(); ++itor1, ++bin_num1) {
    int bin_num2 = 0;
    for (Hist2D::ConstIterator2 itor2 = itor1->begin(); itor2 != itor1->end(); ++itor2, ++bin_num2) {
      if (10 != *itor2) {
        std::cerr << msg << "'s bin number (" << bin_num1 << ", " << bin_num2 << ") has " <<
          *itor2 << " counts, not 10" << std::endl;
        m_failed = true;
      }
    }
  }
}

void EvtBinTest::testLightCurve() {
  using namespace evtbin;

  // Open input table:
  const tip::Table * table = tip::IFileSvc::instance().readTable(m_data_dir + "D1.fits", "EVENTS");

  // Create light curve object.
  LightCurve lc(LinearBinner(0., 900000., 900., "TIME"));

  // Fill the light curve.
  lc.binInput(table->begin(), table->end());

  // Cull keywords from input table.
  lc.harvestKeywords(table->getHeader());

  // Write the light curve to an output file.
  lc.writeOutput("test_evtbin", "LC1.lc");

  // Clean up input.
  delete table;
}

void EvtBinTest::testSingleSpectrum() {
  using namespace evtbin;

  // Open input table:
  const tip::Table * table = tip::IFileSvc::instance().readTable(m_data_dir + "D1.fits", "EVENTS");

  // Create spectrum object.
  SingleSpec spectrum(LogBinner(1., 90000., 1000, "ENERGY"));

  // Fill the spectrum.
  spectrum.binInput(table->begin(), table->end());

  // Cull keywords from input table.
  spectrum.harvestKeywords(table->getHeader());

  // Write the spectrum to an output file.
  spectrum.writeOutput("test_evtbin", "PHA1.pha");

  // Clean up input.
  delete table;
}

void EvtBinTest::testMultiSpectra() {
  using namespace evtbin;

  // Open input table:
  const tip::Table * table = tip::IFileSvc::instance().readTable(m_data_dir + "D1.fits", "EVENTS");

  // Create spectrum object.
  MultiSpec spectrum(LinearBinner(0., 900000., 300000., "TIME"), LogBinner(1., 90000., 1000, "ENERGY"));

  // Fill the spectrum.
  spectrum.binInput(table->begin(), table->end());

  // Cull keywords from input table.
  spectrum.harvestKeywords(table->getHeader());

  // Write the spectrum to an output file.
  spectrum.writeOutput("test_evtbin", "PHA2.pha");

  // Clean up input.
  delete table;
}

void EvtBinTest::testCountMap() {
  using namespace evtbin;

  // Open input table:
  const tip::Table * table = tip::IFileSvc::instance().readTable(m_data_dir + "D1.fits", "EVENTS");

  // Create map object with invalid projection.
  try {
    CountMap count_map(240., 40., "GARBAGE", 400, 200, .1, 0., true);
    std::cerr << "CountMap's constructor did not throw an exception when given an invalid projection type" << std::endl;
    m_failed = true;
  } catch (const std::exception & x) {
    // OK, supposed to fail.
  }

  // Create count map object.
  CountMap count_map(60., 40., "AIT", 400, 400, .2, 0., true);

  // Fill the count map.
  count_map.binInput(table->begin(), table->end());

  // Cull keywords from input table.
  count_map.harvestKeywords(table->getHeader());

  // Write the count map to an output file.
  count_map.writeOutput("test_evtbin", "CM2.fits");

  // Clean up input.
  delete table;
}

/// \brief Create factory singleton object which will create the application:
st_app::StAppFactory<EvtBinTest> g_app_factory;
