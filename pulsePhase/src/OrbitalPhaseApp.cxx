/** \file OrbitalPhaseApp.cxx
    \brief Implmentation of OrbitalPhaseApp class.
    \author Masaharu Hirayama, GSSC
            James Peachey, HEASARC/GSSC
*/
#include "OrbitalPhaseApp.h"

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

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_stream/Stream.h"

const std::string s_cvs_id("$Name$");

OrbitalPhaseApp::~OrbitalPhaseApp() throw() {}

OrbitalPhaseApp::OrbitalPhaseApp(): pulsarDb::PulsarToolApp(), m_os("OrbitalPhaseApp", "", 2) {
  setName("gtophase");
  setVersion(s_cvs_id);
}

void OrbitalPhaseApp::runApp() {
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
  par_group.Prompt("ra");
  par_group.Prompt("dec");
  par_group.Prompt("srcposition");
  par_group.Prompt("strict");
  par_group.Prompt("solareph");
  par_group.Prompt("matchsolareph");
  par_group.Prompt("angtol");
  par_group.Prompt("ophasefield");
  par_group.Prompt("ophaseoffset");
  par_group.Prompt("reportephstatus");

  par_group.Prompt("chatter");
  par_group.Prompt("clobber");
  par_group.Prompt("debug");
  par_group.Prompt("gui");
  par_group.Prompt("mode");

  par_group.Save();

  // Open the event file(s).
  openEventFile(par_group, false);

  // Handle leap seconds.
  std::string leap_sec_file = par_group["leapsecfile"];
  timeSystem::TimeSystem::setDefaultLeapSecFileName(leap_sec_file);

  // Setup time correction mode.
  defineTimeCorrectionMode("DEFAULT", REQUIRED, REQUIRED, SUPPRESSED);
  selectTimeCorrectionMode("DEFAULT");

  // Set variables to initialize ephemeris computations and arrival time corrections.
  std::auto_ptr<pulsarDb::EphChooser> chooser(0);
  std::string eph_style;
  bool vary_ra_dec = false;
  std::string src_position = par_group["srcposition"];
  std::string src_position_uc(src_position);
  for (std::string::iterator itor = src_position_uc.begin(); itor != src_position_uc.end(); ++itor) *itor = std::toupper(*itor);
  if ("USER" == src_position_uc) {
    chooser.reset(new pulsarDb::StrictEphChooser);
    eph_style = "NONE";
    vary_ra_dec = false;

  } else if ("DB" == src_position_uc) {
    bool strict = par_group["strict"];
    if (strict) {
      chooser.reset(new pulsarDb::StrictEphChooser);
    } else {
      chooser.reset(new pulsarDb::SloppyEphChooser);
    }
    eph_style = "DB";
    vary_ra_dec = true;

  } else {
    throw std::runtime_error("Unsupported type of source position \"" + src_position + "\" was specified");
  }

  // Set up EphComputer for arrival time corrections.
  initEphComputer(par_group, *chooser, eph_style, m_os.info(4));

  // Use user input (parameters) together with computer to determine corrections to apply.
  bool guess_pdot = false;
  initTimeCorrection(par_group, vary_ra_dec, guess_pdot, m_os.info(3), "START");

  // Report ephemeris status.
  std::set<pulsarDb::EphStatusCodeType> code_to_report;
  code_to_report.insert(pulsarDb::Remarked);
  reportEphStatus(m_os.warn(), code_to_report);

  // Reserve output column for creation if not existing in the event file(s).
  std::string phase_field = par_group["ophasefield"];
  reserveOutputField(phase_field, "1D");

  // Get EphComputer for orbital phase computation.
  pulsarDb::EphComputer & computer(getEphComputer());

  // Read global phase offset.
  double phase_offset = par_group["ophaseoffset"];

  // Iterate over events.
  for (setFirstEvent(); !isEndOfEventList(); setNextEvent()) {
    // Get event time as AbsoluteTime.
    timeSystem::AbsoluteTime abs_evt_time(getEventTime());

    // Compute phase.
    double phase = computer.calcOrbitalPhase(abs_evt_time, phase_offset);

    // Write phase into output column.
    setFieldValue(phase_field, phase);
  }

  // Write parameter values to the event file(s).
  std::string creator_name = getName() + " " + getVersion();
  std::string file_modification_time(createUtcTimeString());
  std::string header_line("File modified by " + creator_name + " on " + file_modification_time);
  writeParameter(par_group, header_line);
}
