/** \file test_pulsePhase.cxx
    \brief Test code for pulsePhase package.
    \authors Masaharu Hirayama, GSSC,
             James Peachey, HEASARC/GSSC
*/
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <set>
#include <string>

#include "OrbitalPhaseApp.h"
#include "PulsePhaseApp.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "timeSystem/EventTimeHandler.h"
#include "timeSystem/GlastTimeHandler.h"
#include "timeSystem/PulsarTestApp.h"

#include "tip/IFileSvc.h"
#include "tip/TipFile.h"

static const std::string s_cvs_id("$Name$");

/** \class PulsePhaseAppTester
    \brief Test PulsePhaseApp application (gtpphase).
*/
class PulsePhaseAppTester: public timeSystem::PulsarApplicationTester {
  public:
  /** \brief Construct a PulsePhaseAppTester object.
      \param test_app Unit test appliction of pulsar tool package, under which this application tester is to run.
  */
  PulsePhaseAppTester(timeSystem::PulsarTestApp & test_app);

  /// \brief Destruct this PulsePhaseAppTester object.
  virtual ~PulsePhaseAppTester() throw() {}

  /// \brief Returns an application object to be tested.
  virtual st_app::StApp * createApplication() const;

  /** \brief Return a logical true if the given header keyword is determined correct, and a logical false otherwise.
      \param keyword_name Name of the header keyword to be verified.
      \param out_keyword Header keyword taken from the output file to be verified.
      \param ref_keyword Header keyword taken from the reference file which out_keyword is checked against.
      \param error_stream Output stream for this method to put an error messages when verification fails.
  */
  virtual bool verify(const std::string & keyword_name, const tip::KeyRecord & out_keyword,
    const tip::KeyRecord & ref_keyword, std::ostream & error_stream) const;

  /** \brief Return a logical true if the given table cell is considered correct, and a logical false otherwise.
      \param column_name Name of the FITS column that the given table cell belongs to.
      \param out_cell Table cell taken from the output file to be verified.
      \param ref_cell Table cell taken from the reference file which out_cell is checked against.
      \param error_stream Output stream for this method to put an error message when verification fails.
  */
  virtual bool verify(const std::string & column_name, const tip::TableCell & out_cell, const tip::TableCell & ref_cell,
    std::ostream & error_stream) const;

  /** \brief Return a logical true if the given character string is considered correct, and a logical false otherwise.
      \param out_string Character string taken from the output file to be verified.
      \param ref_string Character string taken from the reference file which out_string is checked against.
      \param error_stream Output stream for this method to put an error message when verification fails.
  */
  virtual bool verify(const std::string & out_string, const std::string & ref_string, std::ostream & error_stream) const;
};

PulsePhaseAppTester::PulsePhaseAppTester(timeSystem::PulsarTestApp & test_app): PulsarApplicationTester("gtpphase", test_app) {}

st_app::StApp * PulsePhaseAppTester::createApplication() const {
  return new PulsePhaseApp();
}

bool PulsePhaseAppTester::verify(const std::string & /* keyword_name */, const tip::KeyRecord & out_keyword,
  const tip::KeyRecord & ref_keyword, std::ostream & error_stream) const {
  // Extract keyword values as character strings.
  std::string out_value = out_keyword.getValue();
  std::string ref_value = ref_keyword.getValue();

  // Require an exact match.
  bool verified = (out_value == ref_value);
  if (!verified) error_stream << "Value \"" << out_value << "\" not identical to reference \"" << ref_value << "\".";

  // Return the result.
  return verified;
}

bool PulsePhaseAppTester::verify(const std::string & column_name, const tip::TableCell & out_cell,
  const tip::TableCell & ref_cell, std::ostream & error_stream) const {
  // Initialize return value.
  bool verified = false;

  if ("PULSE_PHASE" == column_name) {
    // Extract cell values as floating-point numbers.
    double out_value;
    double ref_value;
    out_cell.get(out_value);
    ref_cell.get(ref_value);
    error_stream.precision(std::numeric_limits<double>::digits10);

    // Require a match down to the 3rd decimal point.
    double abs_tol = 1.e-3;
    verified = (std::fabs(out_value - ref_value) <= abs_tol);
    if (!verified) {
      error_stream << "Pulse phase " << out_value << " not equivalent to reference " << ref_value <<
        " with absolute tolerance of " << abs_tol << ".";
    }

  } else {
    // Ignore other columns.
    verified = true;
  }

  // Return the result.
  return verified;
}

bool PulsePhaseAppTester::verify(const std::string & out_string, const std::string & ref_string, std::ostream & error_stream) const {
  // Initialize return value.
  bool verified = false;

  if (ref_string.find("MJD") != std::string::npos) {
    // Require a match down to the 10th decimal point, which is approx. 10 microseconds.
    verified = equivalent(out_string, ref_string, 1.e-10, 0.);
    if (!verified) {
      error_stream << "MJD number not equivalent to reference with absolute tolerance of 1e-10 days (8.64 microseconds)." <<
        std::endl << "[OUT] " << out_string << std::endl << "[REF] " << ref_string;
    }

  } else {
    // Require an exact match.
    verified = (out_string == ref_string);
    if (!verified) {
      error_stream << "Line not identical to reference." <<
        std::endl << "[OUT] " << out_string << std::endl << "[REF] " << ref_string;
    }
  }

  // Return the result.
  return verified;
}

/** \class OrbitalPhaseAppTester
    \brief Test OrbitalPhaseApp application (gtophase).
*/
class OrbitalPhaseAppTester: public timeSystem::PulsarApplicationTester {
  public:
  /** \brief Construct a OrbitalPhaseAppTester object.
      \param test_app Unit test appliction of pulsar tool package, under which this application tester is to run.
  */
  OrbitalPhaseAppTester(timeSystem::PulsarTestApp & test_app);

  /// \brief Destruct this OrbitalPhaseAppTester object.
  virtual ~OrbitalPhaseAppTester() throw() {}

  /// \brief Returns an application object to be tested.
  virtual st_app::StApp * createApplication() const;

  /** \brief Return a logical true if the given header keyword is determined correct, and a logical false otherwise.
      \param keyword_name Name of the header keyword to be verified.
      \param out_keyword Header keyword taken from the output file to be verified.
      \param ref_keyword Header keyword taken from the reference file which out_keyword is checked against.
      \param error_stream Output stream for this method to put an error messages when verification fails.
  */
  virtual bool verify(const std::string & keyword_name, const tip::KeyRecord & out_keyword,
    const tip::KeyRecord & ref_keyword, std::ostream & error_stream) const;

  /** \brief Return a logical true if the given table cell is considered correct, and a logical false otherwise.
      \param column_name Name of the FITS column that the given table cell belongs to.
      \param out_cell Table cell taken from the output file to be verified.
      \param ref_cell Table cell taken from the reference file which out_cell is checked against.
      \param error_stream Output stream for this method to put an error message when verification fails.
  */
  virtual bool verify(const std::string & column_name, const tip::TableCell & out_cell, const tip::TableCell & ref_cell,
    std::ostream & error_stream) const;

  /** \brief Return a logical true if the given character string is considered correct, and a logical false otherwise.
      \param out_string Character string taken from the output file to be verified.
      \param ref_string Character string taken from the reference file which out_string is checked against.
      \param error_stream Output stream for this method to put an error message when verification fails.
  */
  virtual bool verify(const std::string & out_string, const std::string & ref_string, std::ostream & error_stream) const;
};

OrbitalPhaseAppTester::OrbitalPhaseAppTester(timeSystem::PulsarTestApp & test_app): PulsarApplicationTester("gtophase", test_app) {}

st_app::StApp * OrbitalPhaseAppTester::createApplication() const {
  return new OrbitalPhaseApp();
}

bool OrbitalPhaseAppTester::verify(const std::string & /* keyword_name */, const tip::KeyRecord & out_keyword,
  const tip::KeyRecord & ref_keyword, std::ostream & error_stream) const {
  // Extract keyword values as character strings.
  std::string out_value = out_keyword.getValue();
  std::string ref_value = ref_keyword.getValue();

  // Require an exact match.
  bool verified = (out_value == ref_value);
  if (!verified) error_stream << "Value \"" << out_value << "\" not identical to reference \"" << ref_value << "\".";

  // Return the result.
  return verified;
}

bool OrbitalPhaseAppTester::verify(const std::string & column_name, const tip::TableCell & out_cell,
  const tip::TableCell & ref_cell, std::ostream & error_stream) const {
  // Initialize return value.
  bool verified = false;

  if ("ORBITAL_PHASE" == column_name) {
    // Extract cell values as floating-point numbers.
    double out_value;
    double ref_value;
    out_cell.get(out_value);
    ref_cell.get(ref_value);
    error_stream.precision(std::numeric_limits<double>::digits10);

    // Require them be close enough as floating-point numbers of type double whose value is of the order of unity.
    double abs_tol = 1.e-9;
    verified = (std::fabs(out_value - ref_value) <= abs_tol);
    if (!verified) {
      error_stream << "Orbital phase " << out_value << " not equivalent to reference " << ref_value <<
        " with absolute tolerance of " << abs_tol << ".";
    }

  } else {
    // Ignore other columns.
    verified = true;
  }

  // Return the result.
  return verified;
}

bool OrbitalPhaseAppTester::verify(const std::string & out_string, const std::string & ref_string, std::ostream & error_stream) const {
  // Initialize return value.
  bool verified = false;

  if (ref_string.find("MJD") != std::string::npos) {
    // Require a match down to the 10th decimal point, which is approx. 10 microseconds.
    verified = equivalent(out_string, ref_string, 1.e-10, 0.);
    if (!verified) {
      error_stream << "MJD number not equivalent to reference with absolute tolerance of 1e-10 days (8.64 microseconds)." <<
        std::endl << "[OUT] " << out_string << std::endl << "[REF] " << ref_string;
    }

  } else {
    // Require an exact match.
    verified = (out_string == ref_string);
    if (!verified) {
      error_stream << "Line not identical to reference." <<
        std::endl << "[OUT] " << out_string << std::endl << "[REF] " << ref_string;
    }
  }

  // Return the result.
  return verified;
}

/** \class PulsePhaseTestApp
    \brief Test pulsePhase package and applications in it.
*/
class PulsePhaseTestApp : public timeSystem::PulsarTestApp {
  public:
    /// \brief Construct a PulsePhaseTestApp object.
    PulsePhaseTestApp();

    /// \brief Destruct this PulsePhaseTestApp object.
    virtual ~PulsePhaseTestApp() throw() {}

    /// \brief Do all tests.
    virtual void runTest();

    /// \brief Test PulsePhaseApp class.
    virtual void testPulsePhaseApp();

    /// \brief Test OrbitalPhaseApp class.
    virtual void testOrbitalPhaseApp();
};

PulsePhaseTestApp::PulsePhaseTestApp(): PulsarTestApp("pulsePhase") {
  setName("test_pulsePhase");
  setVersion(s_cvs_id);
}

void PulsePhaseTestApp::runTest() {
  // Test applications.
  testPulsePhaseApp();
  testOrbitalPhaseApp();
}

void PulsePhaseTestApp::testPulsePhaseApp() {
  setMethod("testPulsePhaseApp");

  // Create an application tester object.
  PulsePhaseAppTester app_tester(*this);

  // List supported event file format(s).
  timeSystem::EventTimeHandlerFactory<timeSystem::GlastScTimeHandler> glast_sctime_handler;

  // Prepare variables to create application objects.
  std::list<std::string> test_name_cont;
  test_name_cont.push_back("par1");
  test_name_cont.push_back("par2");
  test_name_cont.push_back("par3");
  test_name_cont.push_back("par4");
  test_name_cont.push_back("par5");
  test_name_cont.push_back("par6");
  test_name_cont.push_back("par7");
  test_name_cont.push_back("par8");
  test_name_cont.push_back("par9");
  test_name_cont.push_back("par10");

  // Prepare files to be used in the tests.
  std::string ev_file = prependDataPath("my_pulsar_events_v3.fits");
  std::string sc_file = prependDataPath("my_pulsar_spacecraft_data_v3r1.fits");
  std::string master_pulsardb = prependDataPath("master_pulsardb_v2.fits");
  std::string ev_file_2gti = prependDataPath("my_pulsar_events_2gti.fits");

  // Loop over parameter sets.
  for (std::list<std::string>::const_iterator test_itor = test_name_cont.begin(); test_itor != test_name_cont.end(); ++test_itor) {
    const std::string & test_name = *test_itor;
    std::string log_file(getMethod() + "_" + test_name + ".log");
    std::string log_file_ref(prependOutrefPath(log_file));
    std::string out_file(getMethod() + "_" + test_name + ".fits");
    std::string out_file_ref(prependOutrefPath(out_file));

    // Set default parameters.
    st_app::AppParGroup pars(app_tester.getName());
    pars["evfile"] = "";
    pars["scfile"] = "";
    pars["psrdbfile"] = "";
    pars["psrname"] = "ANY";
    pars["ephstyle"] = "DB";
    pars["ephepoch"] = "0.";
    pars["timeformat"] = "FILE";
    pars["timesys"] = "FILE";
    pars["ra"] = 0.;
    pars["dec"] = 0.;
    pars["phi0"] = 0.;
    pars["f0"] = 1.;
    pars["f1"] = 0.;
    pars["f2"] = 0.;
    pars["p0"] = 1.;
    pars["p1"] = 0.;
    pars["p2"] = 0.;
    pars["tcorrect"] = "AUTO";
    pars["solareph"] = "JPL DE405";
    pars["matchsolareph"] = "ALL";
    pars["angtol"] = 1.e-8;
    pars["evtable"] = "EVENTS";
    pars["timefield"] = "TIME";
    pars["sctable"] = "SC_DATA";
    pars["pphasefield"] = "PULSE_PHASE";
    pars["pphaseoffset"] = 0.;
    pars["leapsecfile"] = "DEFAULT";
    pars["reportephstatus"] = "yes";
    pars["chatter"] = 2;
    pars["clobber"] = "yes";
    pars["debug"] = "no";
    pars["gui"] = "no";
    pars["mode"] = "ql";

    // Set test-specific parameters.
    if ("par1" == test_name) {
      // Test standard computation with DB option.
      tip::IFileSvc::instance().openFile(ev_file).copyFile(out_file, true);
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR B0540-69";
      pars["ephstyle"] = "DB";
      pars["psrdbfile"] = master_pulsardb;
      pars["matchsolareph"] = "NONE";
      log_file.erase();
      log_file_ref.erase();

    } else if ("par2" == test_name) {
      // Test standard computation with FREQ option.
      tip::IFileSvc::instance().openFile(ev_file).copyFile(out_file, true);
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR B0540-69";
      pars["ephstyle"] = "FREQ";
      pars["psrdbfile"] = master_pulsardb;
      pars["tcorrect"] = "BARY";
      pars["ra"] = 85.0482;
      pars["dec"] = -69.3319;
      pars["ephepoch"] = 212380785.922;
      pars["timeformat"] = "FILE";
      pars["timesys"] = "TDB";
      pars["phi0"] = 0.1234;
      pars["pphaseoffset"] = -0.1234;
      pars["f0"] = 19.83401688366839422996;
      pars["f1"] = -1.8869945816704768775044e-10;
      pars["f2"] = 0.;
      log_file.erase();
      log_file_ref.erase();

    } else if ("par3" == test_name) {
      // Test phase computation with orbital modulation with DB option.
      tip::IFileSvc::instance().openFile(ev_file).copyFile(out_file, true);
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J1959+2048";
      pars["ephstyle"] = "DB";
      pars["psrdbfile"] = master_pulsardb;
      pars["matchsolareph"] = "NONE";
      log_file.erase();
      log_file_ref.erase();

    } else if ("par4" == test_name) {
      // Test phase computation with orbital modulation with FREQ option.
      tip::IFileSvc::instance().openFile(ev_file).copyFile(out_file, true);
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J1959+2048";
      pars["ephstyle"] = "FREQ";
      pars["psrdbfile"] = master_pulsardb;
      pars["ra"] = 85.0482;
      pars["dec"] = -69.3319;
      pars["ephepoch"] = 212380785.922;
      pars["timeformat"] = "GLAST";
      pars["timesys"] = "TDB";
      pars["phi0"] = 0.;
      pars["f0"] = 19.83401688366839422996;
      pars["f1"] = -1.8869945816704768775044e-10;
      pars["f2"] = 0.;
      pars["matchsolareph"] = "NONE";
      log_file.erase();
      log_file_ref.erase();

    } else if ("par5" == test_name) {
      // Test ephemeris status reporting.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      std::string summary_file("psrdb_summary.txt");
      remove(summary_file.c_str());
      std::ofstream ofs_summary(summary_file.c_str());
      ofs_summary << prependDataPath("psrdb_spin1.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_remark.txt") << std::endl;
      ofs_summary.close();
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["ephstyle"] = "DB";
      pars["psrdbfile"] = "@" + summary_file;
      pars["matchsolareph"] = "NONE";
      out_file.erase();
      out_file_ref.erase();

    } else if ("par6" == test_name) {
      // Test no reporting of ephemeris status with reportephstatus=no.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      std::string summary_file("psrdb_summary.txt");
      remove(summary_file.c_str());
      std::ofstream ofs_summary(summary_file.c_str());
      ofs_summary << prependDataPath("psrdb_spin1.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_remark.txt") << std::endl;
      ofs_summary.close();
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["ephstyle"] = "DB";
      pars["psrdbfile"] = "@" + summary_file;
      pars["matchsolareph"] = "NONE";
      pars["reportephstatus"] = "no";
      out_file.erase();
      out_file_ref.erase();

    } else if ("par7" == test_name) {
      // Test reporting of database creation history.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      std::string summary_file("psrdb_summary.txt");
      remove(summary_file.c_str());
      std::ofstream ofs_summary(summary_file.c_str());
      ofs_summary << prependDataPath("psrdb_spin1.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_remark.txt") << std::endl;
      ofs_summary.close();
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["ephstyle"] = "DB";
      pars["psrdbfile"] = "@" + summary_file;
      pars["matchsolareph"] = "NONE";
      pars["reportephstatus"] = "no";
      pars["chatter"] = 4;
      out_file.erase();
      out_file_ref.erase();

    } else if ("par8" == test_name) {
      // Test reporting of an ephemeris gap which overlaps with the first GTI in the event file.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["ephstyle"] = "DB";
      pars["psrdbfile"] = prependDataPath("psrdb_spin2.txt");
      pars["matchsolareph"] = "NONE";
      out_file.erase();
      out_file_ref.erase();

    } else if ("par9" == test_name) {
      // Test reporting of an ephemeris gap which overlaps with the second GTI in the event file.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["ephstyle"] = "DB";
      pars["psrdbfile"] = prependDataPath("psrdb_spin3.txt");
      pars["matchsolareph"] = "NONE";
      out_file.erase();
      out_file_ref.erase();

    } else if ("par10" == test_name) {
      // Test reporting of an ephemeris gap which overlaps with the both GTI's in the event file.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["ephstyle"] = "DB";
      pars["psrdbfile"] = prependDataPath("psrdb_spin4.txt");
      pars["matchsolareph"] = "NONE";
      out_file.erase();
      out_file_ref.erase();

    } else {
      // Skip this iteration.
      continue;
    }

    // Test the application.
    app_tester.test(pars, log_file, log_file_ref, out_file, out_file_ref);
  }
}

void PulsePhaseTestApp::testOrbitalPhaseApp() {
  setMethod("testOrbitalPhaseApp");

  // Create an application tester object.
  OrbitalPhaseAppTester app_tester(*this);

  // List supported event file format(s).
  timeSystem::EventTimeHandlerFactory<timeSystem::GlastScTimeHandler> glast_sctime_handler;

  // Prepare variables to create application objects.
  std::list<std::string> test_name_cont;
  test_name_cont.push_back("par1");
  test_name_cont.push_back("par2");
  test_name_cont.push_back("par3");
  test_name_cont.push_back("par4");

  // Prepare files to be used in the tests.
  std::string ev_file = prependDataPath("my_pulsar_events_v3.fits");
  std::string sc_file = prependDataPath("my_pulsar_spacecraft_data_v3r1.fits");
  std::string master_pulsardb = prependDataPath("master_pulsardb_v2.fits");
  std::string ev_file_2gti = prependDataPath("my_pulsar_events_2gti.fits");

  // Loop over parameter sets.
  for (std::list<std::string>::const_iterator test_itor = test_name_cont.begin(); test_itor != test_name_cont.end(); ++test_itor) {
    const std::string & test_name = *test_itor;
    std::string log_file(getMethod() + "_" + test_name + ".log");
    std::string log_file_ref(prependOutrefPath(log_file));
    std::string out_file(getMethod() + "_" + test_name + ".fits");
    std::string out_file_ref(prependOutrefPath(out_file));

    // Set default parameters.
    st_app::AppParGroup pars(app_tester.getName());
    pars["evfile"] = "";
    pars["scfile"] = "";
    pars["psrdbfile"] = "";
    pars["psrname"] = "ANY";
    pars["ra"] = 0.;
    pars["dec"] = 0.;
    pars["solareph"] = "JPL DE405";
    pars["matchsolareph"] = "ALL";
    pars["angtol"] = 1.e-8;
    pars["evtable"] = "EVENTS";
    pars["timefield"] = "TIME";
    pars["sctable"] = "SC_DATA";
    pars["ophasefield"] = "ORBITAL_PHASE";
    pars["ophaseoffset"] = 0.;
    pars["leapsecfile"] = "DEFAULT";
    pars["reportephstatus"] = "yes";
    pars["chatter"] = 2;
    pars["clobber"] = "yes";
    pars["debug"] = "no";
    pars["gui"] = "no";
    pars["mode"] = "ql";

    // Set test-specific parameters.
    if ("par1" == test_name) {
      // Test standard computation with DB option.
      tip::IFileSvc::instance().openFile(ev_file).copyFile(out_file, true);
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J1834-0010";
      pars["psrdbfile"] = master_pulsardb;
      pars["ra"] = 85.0482;
      pars["dec"] = -69.3319;
      pars["matchsolareph"] = "NONE";
      log_file.erase();
      log_file_ref.erase();

    } else if ("par2" == test_name) {
      // Test ephemeris status reporting.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      std::string summary_file("psrdb_summary.txt");
      remove(summary_file.c_str());
      std::ofstream ofs_summary(summary_file.c_str());
      ofs_summary << prependDataPath("psrdb_spin1.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_binary.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_remark.txt") << std::endl;
      ofs_summary.close();
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["psrdbfile"] = "@" + summary_file;
      pars["ra"] = 85.0482;
      pars["dec"] = -69.3319;
      pars["matchsolareph"] = "NONE";
      out_file.erase();
      out_file_ref.erase();

    } else if ("par3" == test_name) {
      // Test no reporting of ephemeris status with reportephstatus=no.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      std::string summary_file("psrdb_summary.txt");
      remove(summary_file.c_str());
      std::ofstream ofs_summary(summary_file.c_str());
      ofs_summary << prependDataPath("psrdb_spin1.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_binary.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_remark.txt") << std::endl;
      ofs_summary.close();
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["psrdbfile"] = "@" + summary_file;
      pars["ra"] = 85.0482;
      pars["dec"] = -69.3319;
      pars["matchsolareph"] = "NONE";
      pars["reportephstatus"] = "no";
      out_file.erase();
      out_file_ref.erase();

    } else if ("par4" == test_name) {
      // Test reporting of database creation history.
      tip::IFileSvc::instance().openFile(ev_file_2gti).copyFile(out_file, true);
      std::string summary_file("psrdb_summary.txt");
      remove(summary_file.c_str());
      std::ofstream ofs_summary(summary_file.c_str());
      ofs_summary << prependDataPath("psrdb_spin1.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_binary.txt") << std::endl;
      ofs_summary << prependDataPath("psrdb_remark.txt") << std::endl;
      ofs_summary.close();
      pars["evfile"] = out_file;
      pars["scfile"] = sc_file;
      pars["psrname"] = "PSR J0540-6919";
      pars["psrdbfile"] = "@" + summary_file;
      pars["ra"] = 85.0482;
      pars["dec"] = -69.3319;
      pars["matchsolareph"] = "NONE";
      pars["reportephstatus"] = "no";
      pars["chatter"] = 4;
      out_file.erase();
      out_file_ref.erase();

    } else {
      // Skip this iteration.
      continue;
    }

    // Test the application.
    app_tester.test(pars, log_file, log_file_ref, out_file, out_file_ref);
  }
}

st_app::StAppFactory<PulsePhaseTestApp> g_factory("test_pulsePhase");
