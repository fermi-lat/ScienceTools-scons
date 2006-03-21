#include <cctype>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "pulsarDb/AbsoluteTime.h"
#include "pulsarDb/EphChooser.h"
#include "pulsarDb/EphComputer.h"
#include "pulsarDb/GlastTime.h"
#include "pulsarDb/OrbitalEph.h"
#include "pulsarDb/PulsarDb.h"
#include "pulsarDb/PulsarEph.h"
#include "pulsarDb/TimingModel.h"

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_facilities/Env.h"

using namespace pulsarDb;

const std::string s_cvs_id("$Name$");

class PulsePhaseApp : public st_app::StApp {
  public:
    PulsePhaseApp();
    virtual void run();
};

PulsePhaseApp::PulsePhaseApp(): st_app::StApp() {
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

  // Open the event file.
  tip::Table * events = tip::IFileSvc::instance().editTable(par_group["evfile"], par_group["evtable"]);

  double phase_offset = par_group["ophaseoffset"];

  std::string telescope;
  std::string time_sys;

  const tip::Header & header(events->getHeader());
  header["TELESCOP"].get(telescope);
  header["TIMESYS"].get(time_sys);

  if (telescope != "GLAST") throw std::runtime_error("Only GLAST supported for now");

  // Find the pulsar database.
  std::string psrdb_file = par_group["psrdbfile"];
  std::string psrdb_file_uc = psrdb_file;
  for (std::string::iterator itor = psrdb_file_uc.begin(); itor != psrdb_file_uc.end(); ++itor) *itor = toupper(*itor);
  if ("DEFAULT" == psrdb_file_uc) {
    using namespace st_facilities;
    psrdb_file = Env::appendFileName(Env::getDataDir("pulsePhase"), "master_pulsardb.fits");
  }
  std::string psr_name = par_group["psrname"];
  
  EphComputer computer;

  // Open the database.
  PulsarDb database(psrdb_file);

  // Select only ephemerides for this pulsar.
  database.filterName(psr_name);

  // Load the selected ephemerides.
  computer.load(database);

  std::string time_field = par_group["timefield"];

  // Add orbital phase field if missing.
  bool add_col = true;
  std::string phase_field = par_group["ophasefield"];
  try {
    events->getFieldIndex(phase_field);
    add_col = false;
  } catch (const tip::TipException &) {
  }
  if (add_col)
    events->appendField(phase_field, "1D");

  // Iterate over events.
  for (tip::Table::Iterator itor = events->begin(); itor != events->end(); ++itor) {
    // Get value from the table.
    double evt_time = (*itor)[time_field].get();

    std::auto_ptr<AbsoluteTime> abs_evt_time(0);

    if (time_sys == "TDB") {
      abs_evt_time.reset(new GlastTdbTime(evt_time));
    } else {
      abs_evt_time.reset(new GlastTtTime(evt_time));
    }

    double int_part; // Ignored. Needed for modf.
    double phase = modf(computer.calcOrbitalPhase(*abs_evt_time) + phase_offset, &int_part);

    // Write phase into output column.
    (*itor)[phase_field].set(phase);
  }

  // Clean up.
  delete events;
}

st_app::StAppFactory<PulsePhaseApp> g_factory("gtophase");
