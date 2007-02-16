/** \file test_psearch.cxx
    \brief Period search tool.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#include <cmath>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "pulsarDb/EphChooser.h"
#include "pulsarDb/EphComputer.h"
#include "pulsarDb/PulsarDb.h"
#include "pulsarDb/PulsarEph.h"
#include "pulsarDb/TimingModel.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_facilities/Env.h"

#include "st_stream/Stream.h"
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

#include "timeSystem/AbsoluteTime.h"
#include "timeSystem/TimeRep.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "periodSearch/PeriodTest.h"
#include "periodSearch/PeriodSearchPlotter.h"
#include "ChiSquaredTest.h"
#include "FourierAnalysis.h"
#include "HTest.h"
#include "RayleighTest.h"
#include "Z2nTest.h"

static const std::string s_cvs_id = "$Name$";

class PSearchTestApp : public st_app::StApp {
  public:
    PSearchTestApp(): m_os("PSearchTestApp", "", 2), m_data_dir(), m_failed(false) {
      setName("test_stpsearch");
      setVersion(s_cvs_id);
    }

    virtual ~PSearchTestApp() throw() {}
    virtual void run();

    void testAllStats(double center, double step, long num_trials, double epoch, int num_bins, const std::vector<double> & events,
      double duration, const std::string & unit, bool plot);

    void testFindMax(const periodSearch::PeriodTest & test);

    void testChooseEph(const std::string & ev_file, const std::string & eph_file, const std::string & pulsar_name, double epoch);

    void testFourier(double t_start, double t_stop, double width, int num_bins, const std::vector<double> & events,
      const std::string & unit, bool plot, double min_freq, double max_freq); 

    void testChanceProb();

    const std::string & getDataDir();

    std::string findFile(const std::string & file_name);

    std::string makeTitle(const periodSearch::PeriodTest & test, const std::string & init_title);

  private:
    st_stream::StreamFormatter m_os;
    std::string m_data_dir;
    bool m_failed;
};

void PSearchTestApp::run() {

  // Trick up some fake events.
  int num_evts = 1000;

  std::vector<double> fake_evts(num_evts);
  m_os.out().precision(16);
  m_os.err().precision(16);

  for (int ii = 0; ii < num_evts; ++ii)
    fake_evts[ii] = ii + 1;

  // Set up search parameters.
  double central = 1.;
  double step = .5e-3;
  int num_pds = 61;
  double epoch = .2;
  int num_bins = 10;
  double duration = 1000.;
  std::string unit = "(Hz)";

  // Test process of picking the ephemeris.
  testChooseEph(findFile("ft1tiny.fits"), findFile("groD4-dc2v4.fits"), "crab", 212380785.922);



  if (m_failed) throw std::runtime_error("UNIT TEST FAILED");

  bool plot = getParGroup()["plot"];

  // First do simple test with this highly artificial data.
  testAllStats(central, step, num_pds, epoch, num_bins, fake_evts, duration, unit, plot);

  // Test Fourier analysis of this highly artificial data.
  // Note: width of .1 s -> Nyquist = 1/.2s = 5 Hz.
  testFourier(0., duration, .1, 10000, fake_evts, unit, plot, .9, 1.1);

  // Data taken from M. Hirayama's work with modified ASCA data.
  // http://glast.gsfc.nasa.gov/ssc/dev/psr_tools/existing.html#tryout003
  num_pds = 101;

  central = 1. / 50.41843041e-3;
  step = .168e-7 * central * central;

  epoch = 212380785.922;

  fake_evts.clear();

  // Read some real data.
  std::auto_ptr<const tip::Table> evt_table(tip::IFileSvc::instance().readTable(findFile("step-01.fits"), "EVENTS"));

  // Read telapse for duration from GTI.
  std::auto_ptr<const tip::Table> gti_table(tip::IFileSvc::instance().readTable(findFile("step-01.fits"), "GTI"));
  duration = 0.;
  gti_table->getHeader()["TELAPSE"].get(duration);

  // Make the array big enough to hold these events.
  fake_evts.resize(evt_table->getNumRecords());

  // Correct event times for changed MJDREF.
  timeSystem::MetRep orig_glast_time("TDB", 54101, 0., 0.);
  timeSystem::MetRep current_glast_time("TDB", 51910, 0., 0.);
  std::vector<double>::iterator event_itor = fake_evts.begin();
  for (tip::Table::ConstIterator itor = evt_table->begin(); itor != evt_table->end(); ++itor, ++event_itor) {
    orig_glast_time.setValue((*itor)["TIME"].get());
    current_glast_time = timeSystem::AbsoluteTime(orig_glast_time);
    *event_itor = current_glast_time.getValue();
  }

  double valid_since;
  double valid_until;

  gti_table->getHeader()["TSTART"].get(valid_since);
  gti_table->getHeader()["TSTOP"].get(valid_until);

  // Correct valid_since and valid_until for changed MJDREF.
  orig_glast_time.setValue(valid_since);
  current_glast_time = timeSystem::AbsoluteTime(orig_glast_time);
  valid_since = current_glast_time.getValue();
  orig_glast_time.setValue(valid_until);
  current_glast_time = timeSystem::AbsoluteTime(orig_glast_time);
  valid_until = current_glast_time.getValue();

  // Repeat simple test with this somewhat less artificial data.
  testAllStats(central, step, num_pds, epoch, num_bins, fake_evts, duration, unit, plot);

  // Note: width of .01 s -> Nyquist = 1/.02s = 50 Hz.
  testFourier(valid_since, valid_until, .01, 1000000, fake_evts, unit, plot, 19.82, 19.85);

  // Now test pdot correction.
  double phi0 = 0.; // Ignored for these purposes anyway.
  double pdot = 4.7967744e-13;
  double p2dot = 0.; // Not available.

  using namespace pulsarDb;
  using namespace timeSystem;

  // The following declarator looks like a function prototype.
  // PeriodEph eph(GlastTdbTime(valid_since), GlastTdbTime(valid_until), GlastTdbTime(epoch), phi0, 1. / central, pdot, p2dot);
  // Resolve the misunderstanding by using a temporary variable for the first argument.
  MetRep glast_tdb("TDB", 51910, 0., 0.);
  glast_tdb.setValue(valid_since);
  AbsoluteTime abs_since(glast_tdb);
  glast_tdb.setValue(valid_until);
  AbsoluteTime abs_until(glast_tdb);
  glast_tdb.setValue(epoch);
  AbsoluteTime abs_epoch(glast_tdb);

  PeriodEph eph("TDB", abs_since, abs_until, abs_epoch, phi0, 1. / central, pdot, p2dot);
  TimingModel timing_model;

  // Correct the data.
  for (std::vector<double>::iterator itor = fake_evts.begin(); itor != fake_evts.end(); ++itor) {
    glast_tdb.setValue(*itor);
    AbsoluteTime evt_time(glast_tdb);
    timing_model.cancelPdot(eph, evt_time);
    glast_tdb = evt_time;
    *itor = glast_tdb.getValue();
  }

  // Repeat test with the pdot corrected data.
  testAllStats(central, step, num_pds, epoch, num_bins, fake_evts, duration, unit, plot);

  // Test process of picking the ephemeris.
  testChooseEph(findFile("ft1tiny.fits"), findFile("groD4-dc2v4.fits"), "crab", epoch);

  // Test computations of chance probability.
  testChanceProb();
}

void PSearchTestApp::testAllStats(double center, double step, long num_trials, double epoch, int num_bins,
  const std::vector<double> & events, double duration, const std::string & unit, bool plot) {
  m_os.setMethod("testAllStats");

  // Test ChiSquared case.
  ChiSquaredTest test(center, step, num_trials, epoch, num_bins, duration);

  // Iterate over the fake events.
  for (std::vector<double>::const_iterator itor = events.begin(); itor != events.end(); ++itor) {
    test.fill(*itor);
  }

  test.computeStats();

  periodSearch::PeriodSearchPlotter plotter;

  m_os.out() << "Chi Squared Statistic" << std::endl;
  m_os.out() << test << std::endl;
  if (plot) plotter.plot(test, "Chi Squared Statistic", unit);

  // Test Z2n case.
  Z2nTest test_z2n(center, step, num_trials, epoch, num_bins, duration);

  // Iterate over the fake events.
  for (std::vector<double>::const_iterator itor = events.begin(); itor != events.end(); ++itor) {
    test_z2n.fill(*itor);
  }

  test_z2n.computeStats();

  m_os.out() << "Z2n Statistic" << std::endl;
  m_os.out() << test_z2n << std::endl;
  if (plot) plotter.plot(test_z2n, "Z2n Statistic", unit);

  // Test Rayleigh case.
  RayleighTest test_rayleigh(center, step, num_trials, epoch, duration);

  // Iterate over the fake events.
  for (std::vector<double>::const_iterator itor = events.begin(); itor != events.end(); ++itor) {
    test_rayleigh.fill(*itor);
  }

  test_rayleigh.computeStats();

  m_os.out() << "Rayleigh Statistic" << std::endl;
  m_os.out() << test_rayleigh << std::endl;
  if (plot) plotter.plot(test_rayleigh, "Rayleigh Statistic", unit);

  // Test H case.
  HTest test_h(center, step, num_trials, epoch, num_bins, duration);

  // Iterate over the fake events.
  for (std::vector<double>::const_iterator itor = events.begin(); itor != events.end(); ++itor) {
    test_h.fill(*itor);
  }

  test_h.computeStats();

  m_os.out() << "H Statistic" << std::endl;
  m_os.out() << test_h << std::endl;
  if (plot) plotter.plot(test_h, "H Statistic", unit);
}

void PSearchTestApp::testChooseEph(const std::string & ev_file, const std::string & eph_file, const std::string & pulsar_name,
  double epoch) {
  using namespace pulsarDb;
  using namespace timeSystem;
  using namespace tip;

  m_os.setMethod("testChooseEph");

  // Open event file.
  std::auto_ptr<const Table> ev_table(IFileSvc::instance().readTable(ev_file, "EVENTS"));

  // Need some keywords.
  const Header & header(ev_table->getHeader());
  double mjdref = 0.L;
  header["MJDREF"].get(mjdref);

  // Create a timing model object from which to compute the frequency.
  TimingModel model;
  SloppyEphChooser chooser;

  // Create a computer.
  EphComputer computer(model, chooser);

  // Get database access.
  PulsarDb db(eph_file);

  // Limit database to this pulsar only.
  db.filterName(pulsar_name);

  // Load ephemerides into computer.
  computer.load(db);

  MetRep glast_tdb("TDB", 51910, 0., epoch);
  FrequencyEph freq = computer.calcPulsarEph(AbsoluteTime(glast_tdb));

  const double epsilon = 1.e-8;

  double correct_f0 = 29.93633350069171;
  if (fabs(correct_f0/freq.f0() - 1.) > epsilon) {
    m_failed = true;
    m_os.err() << "f0 was computed to be " << freq.f0() << ", not " << correct_f0 << std::endl;
  }

  double correct_f1 = -3.772519499263467e-10;
  if (fabs(correct_f1/freq.f1() - 1.) > epsilon) {
    m_failed = true;
    m_os.err() << "f1 was computed to be " << freq.f1() << ", not " << correct_f1 << std::endl;
  }

  // Select the best ephemeris for this time.
  glast_tdb.setValue(epoch);
  const PulsarEph & chosen_eph(chooser.choose(computer.getPulsarEphCont(), AbsoluteTime(glast_tdb)));

  double correct_f2 = chosen_eph.f2();
  if (fabs(correct_f2/freq.f2() - 1.) > epsilon) {
    m_failed = true;
    m_os.err() << "ERROR: in testChooseEph, f2 was computed to be " << freq.f2() << ", not " << correct_f2 << std::endl;
  }
}

void PSearchTestApp::testFourier(double t_start, double t_stop, double width, int num_bins, const std::vector<double> & events,
  const std::string & unit, bool plot, double min_freq, double max_freq) {
  m_os.setMethod("testFourier");

  // Create analysis object.
  FourierAnalysis fa(t_start, t_stop, width, num_bins, events.size());

  // Fill the data into the object.
  for (std::vector<double>::const_iterator itor = events.begin(); itor != events.end(); ++itor) {
    fa.fill(*itor);
  }

  // Compute the FFT.
  fa.computeStats();

  m_os.out() << "Fourier Power" << std::endl;
  fa.writeRange(m_os.out(), min_freq, max_freq) << std::endl;

  periodSearch::PeriodSearchPlotter plotter;
  if (plot) plotter.plotRange(fa, "Fourier Power", unit, min_freq, max_freq);

}

void PSearchTestApp::testChanceProb() {
  using namespace periodSearch;

  // Vector to hold array of number of statistically independent trials to test chanceProb.
  std::vector<PeriodSearch::size_type>::size_type trial_size = 11;
  std::vector<PeriodSearch::size_type> num_indep_trial(trial_size, 0);
  num_indep_trial[1] = 1;
  num_indep_trial[2] = 2;
  num_indep_trial[3] = 10;
  for (std::vector<PeriodSearch::size_type>::size_type idx = 4; idx != trial_size; ++idx) {
    num_indep_trial[idx] = 10 * num_indep_trial[idx - 1];
  }

  // Vector to hold array of single-trial probabilities used to test chanceProb calculation.
  std::vector<double>::size_type prob_size = 201;
  std::vector<double> prob_one_trial(prob_size, 0.);
  for (std::vector<double>::size_type idx = 1; idx != prob_size; ++idx) {
    prob_one_trial[idx] = std::pow(.9, double(prob_size - (idx + 1)));
  }

  // Populate array with approximate answers using a standard math library call. Note that this is
  // inaccurate especially for probabilities near 0, and/or for large numbers of trials.
  std::vector<std::vector<double> > approx_chance_prob(trial_size, std::vector<double>(prob_size, 0.));
  for (std::vector<PeriodSearch::size_type>::size_type ii = 0; ii != trial_size; ++ii) {
    for (std::vector<double>::size_type jj = 0; jj != prob_size; ++jj) {
      approx_chance_prob[ii][jj] = 1. - std::pow(1. - prob_one_trial[jj], double(num_indep_trial[ii]));
    }
  }

  // Require the agreement between the approximate simple formula and the form used in the PeriodSearch class
  // to be to about 6.5 digits. Note that this limit cannot be refined because the approximate values are
  // not sufficiently accurate.
  double epsilon = 1.e-7;

  for (std::vector<PeriodSearch::size_type>::size_type ii = 0; ii != trial_size; ++ii) {
    for (std::vector<double>::size_type jj = 0; jj != prob_size; ++jj) {
      double chance_prob = PeriodSearch::chanceProbMultiTrial(prob_one_trial[jj], num_indep_trial[ii]);
      if (0. > chance_prob) {
        m_failed = true;
        m_os.err() << "ERROR: chanceProbMultiTrial(" << prob_one_trial[jj] << ", " << num_indep_trial[ii] <<
          ") unexpectedly returned " << chance_prob << ", which is < 0." << std::endl;
      } else if (1. < chance_prob) {
        m_failed = true;
        m_os.err() << "ERROR: chanceProbMultiTrial(" << prob_one_trial[jj] << ", " << num_indep_trial[ii] <<
          ") unexpectedly returned " << chance_prob << ", which is > 1." << std::endl;
      } else if ((0. == approx_chance_prob[ii][jj] && 0. != chance_prob) ||
        (0. != approx_chance_prob[ii][jj] && epsilon < std::fabs(chance_prob / approx_chance_prob[ii][jj] - 1.))) {
        m_failed = true;
        m_os.err() << "ERROR: chanceProbMultiTrial(" << prob_one_trial[jj] << ", " << num_indep_trial[ii] << ") returned " <<
          chance_prob << ", not " << approx_chance_prob[ii][jj] << ", as expected." << std::endl;
      }
    }
  }
}

const std::string & PSearchTestApp::getDataDir() {
  m_data_dir = st_facilities::Env::getDataDir("periodSearch");
  return m_data_dir;
}

std::string PSearchTestApp::findFile(const std::string & file_name) {
  return st_facilities::Env::appendFileName(getDataDir(), file_name);
}

std::string PSearchTestApp::makeTitle(const periodSearch::PeriodTest & test, const std::string & init_title) {
  std::ostringstream os;
  std::pair<double, double> max = test.findMax();
  std::pair<double, double> chance_prob = test.chanceProb(max.second);

  os << init_title << ", Max at: " << max.first << ", stat: " << max.second;

  // Massage display: if difference between min and max is small enough just use max.
  os.setf(std::ios::scientific);
  os.precision(2); // 2 digits -> < 1. e -4. limit in next line.
  if ((chance_prob.second - chance_prob.first) / chance_prob.second < 1.e-4) 
    os << ", chance prob: " << chance_prob.second;
  else
    os << ", chance prob < " << chance_prob.second;

  return os.str();
}

st_app::StAppFactory<PSearchTestApp> g_factory("test_periodSearch");
