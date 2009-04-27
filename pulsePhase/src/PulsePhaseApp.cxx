/** \file PulsePhaseApp.cxx
    \brief Implmentation of PulsePhaseApp class.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#include "PulsePhaseApp.h"

#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

#include "pulsarDb/EphChooser.h"
#include "pulsarDb/EphComputer.h"
#include "pulsarDb/EphStatus.h"

#include "timeSystem/AbsoluteTime.h"
#include "timeSystem/EventTimeHandler.h"
#include "timeSystem/GlastTimeHandler.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_stream/Stream.h"

using namespace pulsarDb;
using namespace timeSystem;

const std::string s_cvs_id("$Name$");

PulsePhaseApp::PulsePhaseApp(): pulsarDb::PulsarToolApp(), m_os("PulsePhaseApp", "", 2) {
  setName("gtpphase");
  setVersion(s_cvs_id);

  st_app::AppParGroup & par_group = getParGroup(); // getParGroup is in base class st_app::StApp

  par_group.setSwitch("ephstyle");
  par_group.setCase("ephstyle", "FREQ", "ephepoch");
  par_group.setCase("ephstyle", "FREQ", "timeformat");
  par_group.setCase("ephstyle", "FREQ", "timesys");
  par_group.setCase("ephstyle", "FREQ", "ra");
  par_group.setCase("ephstyle", "FREQ", "dec");
  par_group.setCase("ephstyle", "FREQ", "phi0");
  par_group.setCase("ephstyle", "FREQ", "f0");
  par_group.setCase("ephstyle", "FREQ", "f1");
  par_group.setCase("ephstyle", "FREQ", "f2");
  par_group.setCase("ephstyle", "PER", "ephepoch");
  par_group.setCase("ephstyle", "PER", "timeformat");
  par_group.setCase("ephstyle", "PER", "timesys");
  par_group.setCase("ephstyle", "PER", "ra");
  par_group.setCase("ephstyle", "PER", "dec");
  par_group.setCase("ephstyle", "PER", "phi0");
  par_group.setCase("ephstyle", "PER", "p0");
  par_group.setCase("ephstyle", "PER", "p1");
  par_group.setCase("ephstyle", "PER", "p2");
}

PulsePhaseApp::~PulsePhaseApp() throw() {}

void PulsePhaseApp::runApp() {
  m_os.setMethod("runApp()");
  st_app::AppParGroup & par_group = getParGroup(); // getParGroup is in base class st_app::StApp

  // Prompt for selected parameters.
  par_group.Prompt("evfile");
  par_group.Prompt("evtable");
  par_group.Prompt("timefield");
  par_group.Prompt("scfile");
  par_group.Prompt("sctable");
  par_group.Prompt("psrdbfile");
  par_group.Prompt("psrname");
  par_group.Prompt("ephstyle");

  std::string eph_style = par_group["ephstyle"];
  for (std::string::iterator itor = eph_style.begin(); itor != eph_style.end(); ++itor) *itor = toupper(*itor);
  if (eph_style == "FREQ") {
    par_group.Prompt("ephepoch");
    par_group.Prompt("timeformat");
    par_group.Prompt("timesys");
    par_group.Prompt("ra");
    par_group.Prompt("dec");
    par_group.Prompt("phi0");
    par_group.Prompt("f0");
    par_group.Prompt("f1");
    par_group.Prompt("f2");
  } else if (eph_style == "PER") {
    par_group.Prompt("ephepoch");
    par_group.Prompt("timeformat");
    par_group.Prompt("timesys");
    par_group.Prompt("ra");
    par_group.Prompt("dec");
    par_group.Prompt("phi0");
    par_group.Prompt("p0");
    par_group.Prompt("p1");
    par_group.Prompt("p2");
  } else if (eph_style == "DB") {
    // No action needed.
  } else {
    throw std::runtime_error("Ephemeris style \"" + eph_style + "\" is not supported");
  }

  par_group.Prompt("tcorrect");
  par_group.Prompt("solareph");
  par_group.Prompt("matchsolareph");
  par_group.Prompt("angtol");
  par_group.Prompt("pphasefield");
  par_group.Prompt("pphaseoffset");
  par_group.Prompt("leapsecfile");
  par_group.Prompt("reportephstatus");
  par_group.Prompt("chatter");
  par_group.Prompt("clobber");
  par_group.Prompt("debug");
  par_group.Prompt("gui");
  par_group.Prompt("mode");

  // Save the values of the parameters.
  par_group.Save();

  // Open the event file(s).
  openEventFile(par_group, false);

  // Handle leap seconds.
  std::string leap_sec_file = par_group["leapsecfile"];
  timeSystem::TimeSystem::setDefaultLeapSecFileName(leap_sec_file);

  // Setup time correction mode.
  defineTimeCorrectionMode("NONE", SUPPRESSED, SUPPRESSED, SUPPRESSED);
  defineTimeCorrectionMode("AUTO", ALLOWED,    ALLOWED,    SUPPRESSED);
  defineTimeCorrectionMode("BARY", REQUIRED,   SUPPRESSED, SUPPRESSED);
  defineTimeCorrectionMode("BIN",  REQUIRED,   REQUIRED,   SUPPRESSED);
  defineTimeCorrectionMode("ALL",  REQUIRED,   REQUIRED,   SUPPRESSED);
  selectTimeCorrectionMode(par_group);

  // Set up EphComputer for arrival time corrections.
  pulsarDb::StrictEphChooser chooser;
  initEphComputer(par_group, chooser, m_os.info(4));

  // Use user input (parameters) together with computer to determine corrections to apply.
  bool vary_ra_dec = true;
  bool guess_pdot = false;
  initTimeCorrection(par_group, vary_ra_dec, guess_pdot, m_os.info(3), "START");

  // Report ephemeris status.
  std::set<EphStatusCodeType> code_to_report;
  code_to_report.insert(Unavailable);
  code_to_report.insert(Remarked);
  reportEphStatus(m_os.warn(), code_to_report);

  // Reserve output column for creation if not existing in the event file(s).
  std::string phase_field = par_group["pphasefield"];
  reserveOutputField(phase_field, "1D");

  // Get EphComputer for orbital phase computation.
  EphComputer & computer(getEphComputer());

  // Read global phase offset.
  double phase_offset = par_group["pphaseoffset"];

  // Iterate over events.
  for (setFirstEvent(); !isEndOfEventList(); setNextEvent()) {
    // Get event time as AbsoluteTime.
    AbsoluteTime abs_evt_time(getEventTime());

    // Compute phase.
    double phase = computer.calcPulsePhase(abs_evt_time, phase_offset);

    // Write phase into output column.
    setFieldValue(phase_field, phase);
  }

  // Write parameter values to the event file(s).
  std::string creator_name = getName() + " " + getVersion();
  std::string file_modification_time(createUtcTimeString());
  std::string header_line("File modified by " + creator_name + " on " + file_modification_time);
  writeParameter(par_group, header_line);
}
