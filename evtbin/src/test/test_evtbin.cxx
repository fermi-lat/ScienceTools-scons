/** \file test_evtbin.cxx
    \brief Event bin test program.
    \author Yasushi Ikebe, GSSC
            James Peachey, HEASARC
*/

#include <iostream>
#include <stdexcept>
#include <string>

// Class encapsulating a 1 dimensional histogram.
#include "evtbin/Hist1D.h"
// Class encapsulating a 2 dimensional histogram.
#include "evtbin/Hist2D.h"
// Class encapsulating description of a binner with linear equal size bins.
#include "evtbin/LinearBinner.h"
// Class encapsulating description of a binner with logarithmically equal size bins.
#include "evtbin/LogBinner.h"
// Application base class.
#include "st_app/StApp.h"
// Factory used by st_app's standard main to create application object.
#include "st_app/StAppFactory.h"

/** \class EvtBinTest
    \brief Application singleton for evtbin test program.
*/
class EvtBinTest : public st_app::StApp {
  public:
    /** \brief Run all tests.
    */
    virtual void run();

    void testLinearBinner();

    void testLogBinner();

    void testHist1D();

    void testHist2D();

  private:
    bool m_failed;
};

void EvtBinTest::run() {
  m_failed = false;

  // Test linear binner:
  testLinearBinner();
  // Test logarithmic binner:
  testLogBinner();
  // Test one dimensional histogram:
  testHist1D();
  // Test two dimensional histogram:
  testHist2D();

  // Report problems, if any.
  if (m_failed) throw std::runtime_error("Unit test failed");
}

void EvtBinTest::testLinearBinner() {
  using namespace evtbin;
  std::string msg;

  // Create a linear binner with 10 bins spanning the interval [0, 100):
  LinearBinner binner(0., 100., 10);

  // Make sure there are really 10 bins:
  msg = "LinearBinner::getNumBins()";
  if (10 != binner.getNumBins()) std::cerr << msg << " returned " << binner.getNumBins() << ", not 10" << std::endl;

  // Make sure values are correctly classified:
  msg = "LinearBinner::computeIndex(";
  for (long ii = 0; ii < 10; ++ii) {
    int value = 9 + ii * 10;
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
}

void EvtBinTest::testLogBinner() {
  using namespace evtbin;
  std::string msg;

  // Create a log binner with 10 bins spanning the interval [1, exp(10.)):
  LogBinner binner(1., exp(10.), 10);

  // Make sure there are really 10 bins:
  msg = "LogBinner::getNumBins()";
  if (10 != binner.getNumBins()) std::cerr << msg << " returned " << binner.getNumBins() << ", not 10" << std::endl;

  // Make sure values are correctly classified:
  msg = "LogBinner::computeIndex(";
  for (long ii = 0; ii < 10; ++ii) {
    double value = .9999999 * exp(ii + 1);
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
  index = binner.computeIndex(exp(10.));
  if (-1 != index) {
    std::cerr << msg << "exp(10.) returned " << index << ", not -1" << std::endl;
    m_failed = true;
  }

  // Left of left endpoint should return index < 0:
  index = binner.computeIndex(0.);
  if (0 <= index) {
    std::cerr << msg << "0.) returned " << index << ", which is >= 0" << std::endl;
    m_failed = true;
  }

  // Right of right endpoint should return index < 0:
  index = binner.computeIndex(1.000001 * exp(10.));
  if (0 <= index) {
    std::cerr << msg << "1.000001 * exp(10.)) returned " << index << ", which is >= 0" << std::endl;
    m_failed = true;
  }
}

void EvtBinTest::testHist1D() {
  using namespace evtbin;
  std::string msg = "Hist1D";

  // Create a linear binner with 10 bins spanning the interval [0, 100):
  LinearBinner binner(0., 100., 10);

  // Create a histogram using this binner:
  Hist1D lin_hist(&binner);

  // Populate this histogram, starting from right of the right endpoint, going to left of left endpoint:
  for (int ii = 100; ii >= -1; --ii) lin_hist.fillBin(ii);

  // Check whether each bin has the right number:
  int bin_num = 0;
  for (Hist1D::ConstIterator itor = lin_hist.begin(); itor != lin_hist.end(); ++itor, ++bin_num) {
    if (10. != *itor) {
      std::cerr << msg << "'s bin number " << bin_num << " has " << *itor << " counts, not 10" << std::endl;
      m_failed = true;
    }
  }
}

void EvtBinTest::testHist2D() {
  using namespace evtbin;
  std::string msg = "Hist2D";

  // Create a linear binner with 10 bins spanning the interval [0, 100):
  LinearBinner binner1(0., 100., 10);

  // Create a log binner with 10 bins spanning the interval [1, exp(10.)):
  LogBinner binner2(1., exp(10.), 10);

  // Create a histogram using this binner:
  Hist2D hist(&binner1, &binner2);

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
      if (10. != *itor2) {
        std::cerr << msg << "'s bin number (" << bin_num1 << ", " << bin_num2 << ") has " <<
          *itor2 << " counts, not 10" << std::endl;
        m_failed = true;
      }
    }
  }
}

/// \brief Create factory object which can create the application:
st_app::StAppFactory<EvtBinTest> g_app_factory;
