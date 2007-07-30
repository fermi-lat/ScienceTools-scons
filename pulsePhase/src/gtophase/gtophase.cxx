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
  setName("gtophase");
  setVersion(s_cvs_id);
}

void PulsePhaseApp::run() {
  st_app::AppParGroup & par_group = getParGroup("gtophase"); // getParGroup is in base class st_app::StApp

  // Prompt for selected parameters.
  par_group.Prompt("evfile");
  par_group.Prompt("evtable");
  par_group.Prompt("psrdbfile");
  par_group.Prompt("psrname");
  par_group.Prompt("timefield");
  par_group.Prompt("ophasefield");
  par_group.Prompt("ophaseoffset");

  par_group.Save();

  // Open the event file(s).
  openEventFile(par_group);

  // Setup time correction mode.
//  defineTimeCorrectionMode("DEFAULT", REQUIRED, REQUIRED, SUPPRESSED);
  // TODO: Uncomment the above and remove the below to fix the bug reported as JIRA PULS-38.
  defineTimeCorrectionMode("DEFAULT", REQUIRED, SUPPRESSED, SUPPRESSED);
  selectTimeCorrectionMode("DEFAULT");

  // Set up EphComputer for arrival time corrections.
  pulsarDb::TimingModel model;
  pulsarDb::StrictEphChooser chooser;
  initEphComputer(par_group, model, chooser, "DB");

  // Reserve output column for creation if not existing in the event file(s).
  std::string phase_field = par_group["ophasefield"];
  reserveOutputField(phase_field, "1D");

  // Get EphComputer for orbital phase computation.
  EphComputer & computer(getEphComputer());

  // Read global phase offset.
  double phase_offset = par_group["ophaseoffset"];

  // Iterate over events.
  for (setFirstEvent(); !isEndOfEventList(); setNextEvent()) {
    // Get event time as AbsoluteTime.
    AbsoluteTime abs_evt_time(getEventTime());

    double int_part; // Ignored. Needed for modf.
    double phase = modf(computer.calcOrbitalPhase(abs_evt_time) + phase_offset, &int_part);

    // Write phase into output column.
    setFieldValue(phase_field, phase);
  }

}

st_app::StAppFactory<PulsePhaseApp> g_factory("gtophase");
