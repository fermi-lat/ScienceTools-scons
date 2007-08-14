#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "pulsarDb/EphChooser.h"
#include "pulsarDb/EphComputer.h"
#include "pulsarDb/OrbitalEph.h"
#include "pulsarDb/PulsarDb.h"
#include "pulsarDb/PulsarEph.h"
#include "pulsarDb/PulsarToolApp.h"
#include "pulsarDb/TimingModel.h"

#include "timeSystem/AbsoluteTime.h"
#include "timeSystem/GlastMetRep.h"
#include "timeSystem/TimeRep.h"

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_facilities/Env.h"

using namespace pulsarDb;
using namespace timeSystem;

const std::string s_cvs_id("$Name$");

class PulsePhaseApp : public pulsarDb::PulsarToolApp {
  public:
    PulsePhaseApp();
    virtual void run();
};

PulsePhaseApp::PulsePhaseApp(): pulsarDb::PulsarToolApp() {
  setName("gtpphase");
  setVersion(s_cvs_id);
}

void PulsePhaseApp::run() {
  // Clean up from any previous runs.
  resetApp();

  st_app::AppParGroup & par_group = getParGroup("gtpphase"); // getParGroup is in base class st_app::StApp

#if 0
  par_group.setSwitch("ephstyle");

  par_group.setCase("ephstyle", "DB", "ephepoch");
  par_group.setCase("ephstyle", "DB", "phi0");

  par_group.setCase("ephstyle", "FREQ", "f0");
  par_group.setCase("ephstyle", "FREQ", "f1");
  par_group.setCase("ephstyle", "FREQ", "f2");

  par_group.setCase("ephstyle", "PER", "p0");
  par_group.setCase("ephstyle", "PER", "p1");
  par_group.setCase("ephstyle", "PER", "p2");

  par_group.Prompt();
#endif

  // Prompt for selected parameters.
  par_group.Prompt("evfile");
  par_group.Prompt("evtable");
  par_group.Prompt("psrdbfile");
  par_group.Prompt("psrname");
  par_group.Prompt("ephstyle");
  par_group.Prompt("demodbin");
  par_group.Prompt("timefield");
  par_group.Prompt("pphasefield");
  par_group.Prompt("pphaseoffset");
  std::string eph_style = par_group["ephstyle"];
  par_group.Save();

  for (std::string::iterator itor = eph_style.begin(); itor != eph_style.end(); ++itor) *itor = toupper(*itor);

  if (eph_style == "FREQ") {
    par_group.Prompt("ephepoch");
    par_group.Prompt("timeformat");
    par_group.Prompt("timesys");
    par_group.Prompt("leapsecfile");
    par_group.Prompt("phi0");
    par_group.Prompt("f0");
    par_group.Prompt("f1");
    par_group.Prompt("f2");
  } else if (eph_style == "PER") {
    par_group.Prompt("ephepoch");
    par_group.Prompt("timeformat");
    par_group.Prompt("timesys");
    par_group.Prompt("leapsecfile");
    par_group.Prompt("phi0");
    par_group.Prompt("p0");
    par_group.Prompt("p1");
    par_group.Prompt("p2");
  } else if (eph_style == "DB") {
    // No action needed.
  } else {
    throw std::runtime_error("Ephemeris style \"" + eph_style + "\" is not supported.");
  }

  par_group.Save(); // Save the values of the parameters.

  // Open the event file(s).
  openEventFile(par_group, false);

  // Setup time correction mode.
  defineTimeCorrectionMode("NONE", SUPPRESSED, SUPPRESSED, SUPPRESSED);
  defineTimeCorrectionMode("AUTO", ALLOWED,    ALLOWED,    SUPPRESSED);
  defineTimeCorrectionMode("BARY", REQUIRED,   SUPPRESSED, SUPPRESSED);
  // TODO: Do we need both BIN and ALL for this tool?
  defineTimeCorrectionMode("BIN",  REQUIRED,   REQUIRED,   SUPPRESSED);
  defineTimeCorrectionMode("ALL",  REQUIRED,   REQUIRED,   SUPPRESSED);
  selectTimeCorrectionMode(par_group);

  // Set up EphComputer for arrival time corrections.
  pulsarDb::TimingModel model;
  pulsarDb::StrictEphChooser chooser;
  initEphComputer(par_group, model, chooser);

  // Use user input (parameters) together with computer to determine corrections to apply.
  bool guess_pdot = false;
  initTimeCorrection(par_group, guess_pdot, "START");

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

    double int_part; // Ignored. Needed for modf.
    double phase = modf(computer.calcPulsePhase(abs_evt_time) + phase_offset, &int_part);

    // Write phase into output column.
    setFieldValue(phase_field, phase);
  }

}

st_app::StAppFactory<PulsePhaseApp> g_factory("gtpphase");
