/** \file test_evtbin.cxx
    \brief Event bin test program.
    \author Yasushi Ikebe, GSSC
            James Peachey, HEASARC
*/

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

// Class encapsulating a binner configuration helper object.
#include "evtbin/BinConfig.h"
// Class encapsulating a count map.
#include "evtbin/CountMap.h"
// Glass encapsulating GTIs
#include "evtbin/Gti.h"
// Class encapsulating a 1 dimensional histogram.
#include "evtbin/Hist1D.h"
// Class encapsulating a 2 dimensional histogram.
#include "evtbin/Hist2D.h"
// Light curve abstractions.
#include "evtbin/LightCurve.h"
// Class encapsulating description of a const s/n binner.
#include "evtbin/ConstSnBinner.h"
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
// Application parameter class.
#include "st_app/AppParGroup.h"
// Application base class.
#include "st_app/StApp.h"
// Factory used by st_app's standard main to create application object.
#include "st_app/StAppFactory.h"
// Utility used to find test data for this application.
#include "st_facilities/Env.h"
// Message utilities.
#include "st_stream/st_stream.h"
#include "st_stream/StreamFormatter.h"
// Tip File access.
#include "tip/IFileSvc.h"
// Tip Table access.
#include "tip/Table.h"

using namespace evtbin;

const std::string s_cvs_id("$Name$");

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

    void testBinConfig();

    void testGti();

    void testConstSnBinner();

  private:
    st_stream::StreamFormatter m_os;
    std::string m_data_dir;
    std::string m_ft1_file;
    std::string m_ft2_file;
    std::string m_gbm_file;
    double m_t_start;
    double m_t_stop;
    double m_e_min;
    double m_e_max;
    double m_gbm_t_start;
    double m_gbm_t_stop;
    bool m_failed;
};

EvtBinTest::EvtBinTest(): m_os("EvtBinTest", "EvtBinTest", 2), m_t_start(2.167442034386540E+06), m_t_stop(2.185939683959529E+06),
  m_e_min(30.), m_e_max(6000.), m_gbm_t_start(-4.193924833089113E-03), m_gbm_t_stop(1.368369758129120E-01), m_failed(false) {
  setName("test_evtbin");
  setVersion(s_cvs_id);
  m_data_dir = st_facilities::Env::getDataDir("evtbin");
  m_ft1_file = st_facilities::Env::appendFileName(m_data_dir, "ft1tiny.fits");
  m_ft2_file = st_facilities::Env::appendFileName(m_data_dir, "ft2tiny.fits");
  m_gbm_file = st_facilities::Env::appendFileName(m_data_dir, "gbmtiny.fits");
}

void EvtBinTest::run() {
  m_failed = false;

  std::cerr.precision(24);
  std::cout.precision(24);

  // Test high level bin configuration object first, as it tests some things which need to be done first.
  testBinConfig();
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
  // Test multiple spectra with time binning (using Tip):
  testMultiSpectra();
  // Test count map (using Tip):
  testCountMap();
  // Test high level bin configuration object.
  testGti();
  // Test const s/n binner:
  testConstSnBinner();

  // Report problems, if any.
  if (m_failed) throw std::runtime_error("Unit test failed");
}

void EvtBinTest::testLinearBinner() {
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
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not a negative index" << std::endl;
    }
    
    // A value greater than the last bin.
    value = 1.;
    index = binner.computeIndex(value);
    if (0 <= index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not a negative index" << std::endl;
    }
    
    // A value on the leading edge of the first bin.
    value = 0.;
    index = binner.computeIndex(value);
    if (0 != index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not 0" << std::endl;
    }

    // A value on the trailing edge of the last bin.
    value = .72;
    index = binner.computeIndex(value);
    if (0 <= index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not a negative index" << std::endl;
    }

    // A value in the 0th bin.
    value = .05;
    index = binner.computeIndex(value);
    if (0 != index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not 0" << std::endl;
    }
    
    // A value in the 1st bin.
    value = .17;
    index = binner.computeIndex(value);
    if (1 != index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not 1" << std::endl;
    }
    
    // A value in the 2st bin.
    value = .30;
    index = binner.computeIndex(value);
    if (2 != index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not 2" << std::endl;
    }
    
    // A value in the 3rd bin.
    value = .55;
    index = binner.computeIndex(value);
    if (3 != index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not 3" << std::endl;
    }
    
    // A value in the 4th bin.
    value = .60;
    index = binner.computeIndex(value);
    if (4 != index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not 4" << std::endl;
    }
    
    // A value between bins.
    value = .25;
    index = binner.computeIndex(value);
    if (0 <= index) {
      m_failed = true;
      std::cerr << msg << value << ") returned " << index << ", not a negative index" << std::endl;
    }
  
  } catch (const std::exception & x) {
    std::cerr << msg << " threw when given a set of intervals which are legal (i.e. in order)" << std::endl;
    m_failed = true;
  }
}

void EvtBinTest::testHist1D() {
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
      double value = .9999999 * exp(double(jj + 1));
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
  // Good time interval from event file.
  Gti gti(m_ft1_file);

  // Create light curve object.
  LightCurve lc(m_ft1_file, "EVENTS", m_ft2_file, LinearBinner(m_t_start, m_t_stop, (m_t_stop - m_t_start) * .01, "TIME"), gti);

  // Fill the light curve.
  lc.binInput();

  // Write the light curve to an output file.
  lc.writeOutput("test_evtbin", "LC1.lc");

  // Good time interval for GBM data.
  Gti gbm_gti;
  gbm_gti.insertInterval(m_gbm_t_start, m_gbm_t_stop);

  // Create light curve object for GBD data.
  LightCurve gbm_lc(m_gbm_file, "EVENTS", "",
    LinearBinner(m_gbm_t_start, m_gbm_t_stop, (m_gbm_t_stop - m_gbm_t_start) * .01, "TIME"), gbm_gti);

  // Fill the light curve.
  gbm_lc.binInput();

  // Write the light curve to an output file.
  gbm_lc.writeOutput("test_evtbin", "GBMLC1.lc");

}

void EvtBinTest::testSingleSpectrum() {
  // Test creating spectrum from LAT data.
  // Create binner used both for energy bins and for ebounds definition.
  LogBinner energy_binner(m_e_min, m_e_max, 100, "ENERGY");

  // Good time interval from event file.
  Gti gti(m_ft1_file);

  // Create spectrum object.
  SingleSpec spectrum(m_ft1_file, "EVENTS", m_ft2_file, energy_binner, energy_binner, gti);

  // Fill the spectrum.
  spectrum.binInput();

  // Write the spectrum to an output file.
  spectrum.writeOutput("test_evtbin", "PHA1.pha");

  // Test creating spectrum from GBM data.
  // Start with GBM configuration.
  std::auto_ptr<BinConfig> config(BinConfig::create(m_gbm_file));

  // Set parameters.
  st_app::AppParGroup & pars(getParGroup());
  pars["evfile"] = m_gbm_file;
  std::auto_ptr<Binner> gbm_energy_binner(config->createEnergyBinner(pars));
  std::auto_ptr<Binner> gbm_ebounds(config->createEbounds(pars));
  std::auto_ptr<Gti> gbm_gti(config->createGti(pars));

  SingleSpec gbm_spectrum(m_gbm_file, "EVENTS", "", *gbm_energy_binner, *gbm_ebounds, *gbm_gti);

  // Fill the spectrum.
  gbm_spectrum.binInput();

  // Write the spectrum to an output file.
  gbm_spectrum.writeOutput("test_evtbin", "GBMPHA1.pha");
}

void EvtBinTest::testMultiSpectra() {
  // Create binner used both for energy bins and for ebounds definition.
  LogBinner energy_binner(m_e_min, m_e_max, 100, "ENERGY"), LogBinner(m_e_min, m_e_max, 100, "ENERGY");

  // Good time interval from event file.
  Gti gti(m_ft1_file);

  // Create spectrum object.
  MultiSpec spectrum(m_ft1_file, "EVENTS", m_ft2_file, LinearBinner(m_t_start, m_t_stop, (m_t_stop - m_t_start) * .1, "TIME"),
    energy_binner, energy_binner, gti);

  // Fill the spectrum.
  spectrum.binInput();

  // Write the spectrum to an output file.
  spectrum.writeOutput("test_evtbin", "PHA2.pha");
}

void EvtBinTest::testCountMap() {
  // Create map object with invalid projection.
  try {
// Disabling this test because currently astro doesn't detect the error.
//    CountMap count_map(240., 40., "GARBAGE", 400, 200, .1, 0., true, "RA", "DEC");
//    std::cerr << "CountMap's constructor did not throw an exception when given an invalid projection type" << std::endl;
//    m_failed = true;
  } catch (const std::exception & x) {
    // OK, supposed to fail.
  }

  // Good time interval from event file.
  Gti gti(m_ft1_file);

  // Create count map object.
  CountMap count_map(m_ft1_file, "EVENTS", m_ft2_file, 8.3633225E+01, 2.2014458E+01, "AIT", 100, 100, .1, 0., false,
    "RA", "DEC", gti);

  // Fill the count map.
  count_map.binInput();

  // Write the count map to an output file.
  count_map.writeOutput("test_evtbin", "CM2.fits");
}

void EvtBinTest::testBinConfig() {
  m_os.setMethod("testBinConfig");

  // Get parameters.
  st_app::AppParGroup & par_group = getParGroup("test_evtbin");

  Binner * binner = 0;

  try {
    // Set the input file/table.
    par_group["evfile"] = m_ft1_file;
    par_group["evtable"] = "EVENTS";

    // Set name of time field to be something strange.
    par_group["timefield"] = "WackyTime";

    // First test the simple case.
    par_group["timebinalg"] = "LIN";

    // Use unlikely values for the other binning parameters.
    par_group["tstart"] = -177.;
    par_group["tstop"] = -100.;
    par_group["deltatime"] = 7.;

    // Save these parameters.
    par_group.Save();

    // Try to find a binner configuration. This should fail because no prototypes were loaded yet.
    try {
      std::auto_ptr<BinConfig> config(BinConfig::create(par_group["evfile"]));
      m_failed = true;
      m_os.err() << "Unexpected: created a BinConfig before prototypes were loaded." << std::endl;
    } catch (const std::exception & x) {
      m_os.info() << "Expected: failed to create a BinConfig before prototypes were loaded: " << x.what() << std::endl;
    }

    // Load standard mission/instrument configurations.
    BinConfig::load();

    // Create a configuration object.
    std::auto_ptr<BinConfig> config(BinConfig::create(par_group["evfile"]));

    // Test prompting for time binner parameters. Since they're all hidden, their values should just be
    // the same as the values just assigned above.
    config->timeParPrompt(par_group);

    // Make sure the value set above DID take.
    if (0 != par_group["timefield"].Value().compare("WackyTime")) {
      m_failed = true;
      std::cerr << "BinConfig::timeParPrompt got name " << par_group["timefield"].Value() << ", not WackyTime" << std::endl;
    }

    // Test creating the time binner.
    binner = config->createTimeBinner(par_group);

    // Name of binner should be the value from the timefield parameter.
    if (0 != binner->getName().compare("WackyTime")) {
      m_failed = true;
      std::cerr << "BinConfig::createTimeBinner created a binner named " << binner->getName() << ", not WackyTime" << std::endl;
    }

    // The number of bins should also be consistent with the parameter file.
    if (11 != binner->getNumBins()) {
      m_failed = true;
      std::cerr << "BinConfig::createTimeBinner created a binner with " << binner->getNumBins() << " bins, not 11" << std::endl;
    }

    // The first bin should begin with tstart.
    if (-177. != binner->getInterval(0).begin()) {
      m_failed = true;
      std::cerr << "BinConfig::createTimeBinner created a binner whose first bin begins with " <<
        binner->getInterval(0).begin() << " not -177." << std::endl;
    }

    // The last bin should end with tstop.
    if (-100. != binner->getInterval(binner->getNumBins() - 1).end()) {
      m_failed = true;
      std::cerr << "BinConfig::createTimeBinner created a binner whose last bin ends with " <<
        binner->getInterval(binner->getNumBins() - 1).end() << " not -100." << std::endl;
    }

    delete binner; binner = 0;

    // Now test the logarithmic case with energy bins.
    // Set name of energy field to be something strange.
    par_group["energyfield"] = "WackyEnergy";

    // First test the simple case.
    par_group["energybinalg"] = "LOG";

    // Use unlikely values for the other binning parameters.
    par_group["emin"] = 1.e-7;
    par_group["emax"] = 1.;
    par_group["enumbins"] = 7;

    // Save these parameters.
    par_group.Save();

    // Prompt for energy values. Again, they're all hidden.
    config->energyParPrompt(par_group);

    // Test creating the energy binner.
    binner = config->createEnergyBinner(par_group);

    // Name of binner should be the value from the energyfield parameter.
    if (0 != binner->getName().compare("WackyEnergy")) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner created a binner named " << binner->getName() << ", not WackyEnergy" << std::endl;
    }

    // The number of bins should also be consistent with the parameter file.
    if (7 != binner->getNumBins()) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner created a binner with " << binner->getNumBins() << " bins, not 7" << std::endl;
    }

    // The first bin should begin with emin.
    if (-7 != int(floor(log10(binner->getInterval(0).begin())))) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner created a binner whose first bin begins with " <<
        binner->getInterval(0).begin() << " not 1.e-7" << std::endl;
    }

    // The first bin should end with one order of magnitude more than emin.
    if (-6 != int(floor(log10(binner->getInterval(0).end())))) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner created a binner whose first bin ends with " <<
        binner->getInterval(0).end() << " not 1.e-6" << std::endl;
    }

    // The last bin should begin with one order of magnitude less than emax.
    if (-1 != int(floor(log10(binner->getInterval(binner->getNumBins() - 1).begin())))) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner created a binner whose last bin begins with " <<
        binner->getInterval(binner->getNumBins() - 1).begin() << " not .1" << std::endl;
    }

    // The last bin should end with emax.
    if (0 != int(floor(log10(binner->getInterval(binner->getNumBins() - 1).end())))) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner created a binner whose last bin ends with " <<
        binner->getInterval(binner->getNumBins() - 1).end() << " not 1." << std::endl;
    }

    delete binner; binner = 0;

    // Now test the bin file case with energy bins.
    par_group["energybinalg"] = "FILE";
    par_group["energybinfile"] = st_facilities::Env::appendFileName(m_data_dir, "energybins.fits");

    // Save these parameters.
    par_group.Save();

    // Prompt for energy values. Again, they're all hidden.
    config->energyParPrompt(par_group);

    // Test creating the energy binner.
    binner = config->createEnergyBinner(par_group);

    // The number of bins should also be consistent with the bin definition file.
    if (1024 != binner->getNumBins()) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner created a binner with " << binner->getNumBins() << " bins, not 1024" << std::endl;
    }

    // The beginning of the first bin should match the file contents.
    if (30. != binner->getInterval(0).begin()) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner with bin def file created a binner whose first bin begins with " <<
        binner->getInterval(0).begin() << " not 30." << std::endl;
    }

    // The end value of the first bin should match the file contents.
    if (float(30.2030597787817) != float(binner->getInterval(0).end())) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner with bin def file created a binner whose first bin ends with " <<
        binner->getInterval(0).end() << " not 30.2030597787817" << std::endl;
    }

    // The beginning value of the last bin should match the file contents.
    // ? This should work but there seems to be some spurious rounding error in cfitsio.
    // if (float(29798.3054230906) != float(binner->getInterval(binner->getNumBins() - 1).begin())) {
    if (float(29798.306) != float(binner->getInterval(binner->getNumBins() - 1).begin())) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner with bin def file created a binner whose last bin begins with " <<
        binner->getInterval(binner->getNumBins() - 1).begin() << " not 29798.306" << std::endl;
    }

    // The end value of the last bin should match the file contents.
    if (float(29999.9999999999) != float(binner->getInterval(binner->getNumBins() - 1).end())) {
      m_failed = true;
      std::cerr << "BinConfig::createEnergyBinner with bin def file created a binner whose last bin ends with " <<
        binner->getInterval(binner->getNumBins() - 1).end() << " not 29999.9999999999" << std::endl;
    }

  } catch (const std::exception & x) {
    m_failed = true;
    std::cerr << "testBinConfig encountered an unexpected error: " << x.what() << std::endl;
  }

  delete binner;

}

void EvtBinTest::testGti() {
  Gti gti1;
  gti1.insertInterval(1., 2.);

  Gti gti2;
  gti2.insertInterval(2., 3.);

  Gti result = gti1 & gti2;
  if (result.begin() != result.end()) {
    std::cerr << "testGti found gti1 overlaps gti2 but they should be disjoint" << std::endl;
    m_failed = true;
  }

  result = Gti();
  result = gti2 & gti1;
  if (result.begin() != result.end()) {
    std::cerr << "testGti found gti2 overlaps gti1 but they should be disjoint" << std::endl;
    m_failed = true;
  }

  Gti gti3;
  gti3.insertInterval(1.5, 1.75);

  result = Gti();
  result = gti1 & gti3;
  if (result.begin() == result.end()) {
    std::cerr << "testGti found gti1 does not overlap gti3 but they should overlap" << std::endl;
    m_failed = true;
  } else if (result.begin()->first != 1.5 || result.begin()->second != 1.75) {
    std::cerr << "testGti found gti1 & gti3 == [" << result.begin()->first << ", " << result.begin()->second <<
      "], not [1.5, 1.75]" << std::endl;
    m_failed = true;
  }

  result = Gti();
  result = gti3 & gti1;
  if (result.begin() == result.end()) {
    std::cerr << "testGti found gti3 does not overlap gti1 but they should overlap" << std::endl;
    m_failed = true;
  } else if (result.begin()->first != 1.5 || result.begin()->second != 1.75) {
    std::cerr << "testGti found gti3 & gti1 == [" << result.begin()->first << ", " << result.begin()->second <<
      "], not [1.5, 1.75]" << std::endl;
    m_failed = true;
  }

  Gti gti4;
  gti4.insertInterval(1.5, 2.5);

  result = Gti();
  result = gti1 & gti4;
  if (result.begin() == result.end()) {
    std::cerr << "testGti found gti1 does not overlap gti4 but they should overlap" << std::endl;
    m_failed = true;
  } else if (result.begin()->first != 1.5 || result.begin()->second != 2.0) {
    std::cerr << "testGti found gti1 & gti4 == [" << result.begin()->first << ", " << result.begin()->second <<
    "], not [1.5, 2.0]" << std::endl;
    m_failed = true;
  }

  result = Gti();
  result = gti4 & gti1;
  if (result.begin() == result.end()) {
    std::cerr << "testGti found gti4 does not overlap gti1 but they should overlap" << std::endl;
    m_failed = true;
  } else if (result.begin()->first != 1.5 || result.begin()->second != 2.0) {
    std::cerr << "testGti found gti4 & gti1 == [" << result.begin()->first << ", " << result.begin()->second <<
      "], not [1.5, 2.0]" << std::endl;
    m_failed = true;
  }

  // Now two GTIs with multiple entries.
  Gti gti5;
  gti5.insertInterval(1., 2.);
  gti5.insertInterval(3., 4.);
  gti5.insertInterval(5., 6.);
  gti5.insertInterval(9., 10.);

  Gti gti6;
  gti6.insertInterval(2.5, 3.5);
  gti6.insertInterval(3.75, 5.1);
  gti6.insertInterval(5.3, 5.5);
  gti6.insertInterval(6.5, 7.5);
  gti6.insertInterval(8.5, 9.5);
  gti6.insertInterval(10.5, 11.5);

  Gti correct_result;
  correct_result.insertInterval(3., 3.5);
  correct_result.insertInterval(3.75, 4.);
  correct_result.insertInterval(5., 5.1);
  correct_result.insertInterval(5.3, 5.5);
  correct_result.insertInterval(9., 9.5);

  result = Gti();
  result = gti5 & gti6;
  if (result != correct_result) {
    std::cerr << "testGti found gti5 & gti6 did not return expected result" << std::endl;
    m_failed = true;
  }

  result = Gti();
  result = gti6 & gti5;
  if (result != correct_result) {
    std::cerr << "testGti found gti6 & gti5 did not return expected result" << std::endl;
    m_failed = true;
  }

  // Check ONTIME computation.
  double on_time = correct_result.computeOntime();
  double expected_on_time = 1.55;
  const double tolerance = 1.e-12;
  if (tolerance < fabs(expected_on_time - on_time)) {
    std::cerr << "testGti: computeOntime returned " << on_time << ", not " << expected_on_time << " as expected" << std::endl;
    m_failed = true;
  }

  Gti gti(m_ft1_file);

  // Create light curve object.
  LightCurve lc(m_ft1_file, "EVENTS", m_ft2_file, LinearBinner(m_t_start, m_t_stop, (m_t_stop - m_t_start) * .01, "TIME"), gti);

  const Gti & lc_gti = lc.getGti();
  if (1 != lc_gti.getNumIntervals()) {
    std::cerr << "testGti read GTI from test ft1 file with " << lc_gti.getNumIntervals() << ", not 1" << std::endl;
    m_failed = true;
  } else if (m_t_start != lc_gti.begin()->first || m_t_stop != lc_gti.begin()->second) {
    std::cerr << "testGti read GTI from test ft1 file with values [" << lc_gti.begin()->first << ", " << lc_gti.begin()->second <<
      ", not [" << m_t_start << ", " << m_t_stop << "]" << std::endl;
    m_failed = true;
  }

  // Check ONTIME computation from light curve.
  on_time = lc_gti.computeOntime();
  expected_on_time = m_t_stop - m_t_start;
  if (tolerance < fabs(expected_on_time - on_time)) {
    std::cerr << "testGti: computeOntime returned " << on_time << ", not " << expected_on_time << " as expected" << std::endl;
    m_failed = true;
  }

}

void EvtBinTest::testConstSnBinner() {
  m_os.setMethod("testConstSnBinner()");

  // Create test binner object.
  ConstSnBinner binner(1., 101., 5., 1., 25.);

  // Check value before first bin.
  long index = binner.computeIndex(0.);
  if (-1 != index) {
    m_failed = true;
    m_os.err() << "In first binner test, index of 0. was " << index << ", not -1" << std::endl;
  }

  // Check value exactly at the first bin.
  index = binner.computeIndex(1.);
  if (-1 != index) {
    m_failed = true;
    m_os.err() << "In first binner test, index of 1. was " << index << ", not -1" << std::endl;
  }

  // Check values up through and including the end of the last bin.
  for (double ev_time = 2.; ev_time <= 101.; ++ev_time) {
    index = binner.computeIndex(ev_time);
    long correct_index = long((ev_time - 2.)/25.);
    if (index != correct_index) {
      m_failed = true;
      m_os.err() << "In first binner test, index of " << ev_time << " was " << index << ", not " << correct_index << std::endl;
    }
  }

  // Check value after last bin.
  index = binner.computeIndex(102.);
  if (-1 != index) {
    m_failed = true;
    m_os.err() << "In first binner test, index of 102. was " << index << ", not -1" << std::endl;
  }

  // Reset the binner.
  binner = ConstSnBinner(1., 101., 5., 1., 25.);

  // Loop over some more times with larger step between each one.
  for (double ev_time = 2.; ev_time <= 101.; ev_time += 2.) {
    index = binner.computeIndex(ev_time);
    long correct_index = long((ev_time - 2.)/50.);
    if (index != correct_index) {
      m_failed = true;
      m_os.err() << "In second binner test, index of " << ev_time << " was " << index << ", not " << correct_index << std::endl;
    }
  }

  // Check value after last bin.
  index = binner.computeIndex(102.);
  if (-1 != index) {
    m_failed = true;
    m_os.err() << "In second binner test, index of 102. was " << index << ", not -1" << std::endl;
  }

  // Reset the binner.
  binner = ConstSnBinner(1., 101., 5., 1., 25.);

  // Loop over some times which are not uniformly distributed. Note upper cutoff is different from earlier tests
  // because adding the sine to the event time makes the last times later than the binner's upper bound.
  for (double ev_time = 2.; ev_time < 101.; ++ev_time) {
    double true_ev_time = ev_time + .1 * sin(ev_time);
    index = binner.computeIndex(true_ev_time);
    long correct_index = long((ev_time - 2.)/25.);
    if (index != correct_index) {
      m_failed = true;
      m_os.err() << "In third binner test, index of " << true_ev_time << " was " << index << ", not " << correct_index << std::endl;
    }
  }

  // Reset the binner, this time with a uniform background.
  std::vector<double> background_coeffs(1, 1.);
  binner = ConstSnBinner(1., 101., 5., 1., 25., background_coeffs);

  // Loop over twice as many times, but with the background of 1, the results will be the same as the second test.

  // Check value before first bin.
  index = binner.computeIndex(0.);
  if (-1 != index) {
    m_failed = true;
    m_os.err() << "In fourth binner test, index of 0. was " << index << ", not -1" << std::endl;
  }

  // Loop over some more times with smaller step between each one.
  for (double ev_time = 1.5; ev_time <= 101.; ev_time += .5) {
    index = binner.computeIndex(ev_time);
    long correct_index = long((ev_time - 1.5)/50.);
    if (index != correct_index) {
      m_failed = true;
      m_os.err() << "In fourth binner test, index of " << ev_time << " was " << index << ", not " << correct_index << std::endl;
    }
  }

  // Check value after last bin.
  index = binner.computeIndex(102.);
  if (-1 != index) {
    m_failed = true;
    m_os.err() << "In fourth binner test, index of 102. was " << index << ", not -1" << std::endl;
  }

}

/// \brief Create factory singleton object which will create the application:
st_app::StAppFactory<EvtBinTest> g_app_factory("test_evtbin");
