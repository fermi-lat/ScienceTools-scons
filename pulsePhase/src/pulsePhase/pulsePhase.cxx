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

using namespace pulsarDb;

const std::string s_cvs_id("$Name$");

class PulsePhaseApp : public st_app::StApp {
  public:
    PulsePhaseApp();
    virtual void run();
};

PulsePhaseApp::PulsePhaseApp(): st_app::StApp() {
  setName("pulsePhase");
  setVersion(s_cvs_id);
}

void PulsePhaseApp::run() {
  st_app::AppParGroup & par_group = getParGroup("pulsePhase"); // getParGroup is in base class st_app::StApp

  // par_group.Prompt(); // Prompts for all parameters.
  par_group.Prompt("evfile");
  par_group.Prompt("evtable");
  par_group.Save();

  par_group.Prompt("ephstyle");
  std::string eph_style = par_group["ephstyle"];

  for (std::string::iterator itor = eph_style.begin(); itor != eph_style.end(); ++itor) *itor = toupper(*itor);

  if (eph_style == "FREQ") {
    par_group.Prompt("epoch");
    par_group.Prompt("phi0");
    par_group.Prompt("f0");
    par_group.Prompt("f1");
    par_group.Prompt("f2");
  } else if (eph_style == "PER") {
    par_group.Prompt("epoch");
    par_group.Prompt("phi0");
    par_group.Prompt("p0");
    par_group.Prompt("p1");
    par_group.Prompt("p2");
  } else if (eph_style == "FILE") {
    par_group.Prompt("psrdbfile");
    par_group.Prompt("psrname");
    par_group.Prompt("binarycorr");
  } else {
    throw std::runtime_error("Ephemeris style \"" + eph_style + "\" is not supported.");
  }

  par_group.Save(); // Save the values of the parameters.

  // Open the event file.
  tip::Table * events = tip::IFileSvc::instance().editTable(par_group["evfile"], par_group["evtable"]);

  // Get model parameters.
  double epoch = par_group["epoch"];
  double user_phi0 = par_group["phi0"];

  // Get keywords.
  double valid_since = 0.;
  double valid_until = 0.;
  std::string telescope;
  std::string time_sys;

  const tip::Header & header(events->getHeader());
  header["TSTART"].get(valid_since);
  header["TSTOP"].get(valid_until);
  header["TELESCOP"].get(telescope);
  header["TIMESYS"].get(time_sys);

  if (telescope != "GLAST") throw std::runtime_error("Only GLAST supported for now");

  std::auto_ptr<AbsoluteTime> abs_epoch(0);
  std::auto_ptr<AbsoluteTime> abs_valid_since(0);
  std::auto_ptr<AbsoluteTime> abs_valid_until(0);
  if (time_sys == "TDB") {
    abs_epoch.reset(new GlastTdbTime(epoch));
    abs_valid_since.reset(new GlastTdbTime(valid_since));
    abs_valid_until.reset(new GlastTdbTime(valid_until));
  } else if (time_sys == "TT") {
    abs_epoch.reset(new GlastTtTime(epoch));
    abs_valid_since.reset(new GlastTtTime(valid_since));
    abs_valid_until.reset(new GlastTtTime(valid_until));
  } else {
    throw std::runtime_error("Only TDB or TT time systems are supported for now");
  }

  EphComputer computer;

  PulsarEphCont & ephemerides(computer.getPulsarEphCont());

  // This phi0 is the phase of the ephemeris itself. The user supplied user_phi0 is a phase offset applied to the whole event file.
  double phi0 = 0.;

  bool binary_corr = false;

  if (eph_style == "FREQ") {
    double f0 = par_group["f0"];
    double f1 = par_group["f1"];
    double f2 = par_group["f2"];

    if (0. >= f0) throw std::runtime_error("Frequency must be positive.");

    ephemerides.push_back(FrequencyEph(*abs_valid_since, *abs_valid_until, *abs_epoch, phi0, f0, f1, f2).clone());
  } else if (eph_style == "PER") {
    double p0 = par_group["p0"];
    double p1 = par_group["p1"];
    double p2 = par_group["p2"];

    if (0. >= p0) throw std::runtime_error("Period must be positive.");

    ephemerides.push_back(PeriodEph(*abs_valid_since, *abs_valid_until, *abs_epoch, phi0, p0, p1, p2).clone());
  } else if (eph_style == "FILE") {
    std::string psrdb_file = par_group["psrdbfile"];
    std::string psr_name = par_group["psrname"];
    binary_corr = par_group["binarycorr"];

    // Open the database.
    PulsarDb database(psrdb_file);

    // Select only ephemerides for this pulsar.
    database.filterName(psr_name);

    // Load the selected ephemerides.
    computer.load(database);
  } else {
    throw std::runtime_error("Ephemeris style \"" + eph_style + "\" is not supported.");
  }

  std::string time_field = par_group["timefield"];

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

    if (binary_corr) {
      computer.demodulateBinary(*abs_evt_time);
    }

    double int_part; // Ignored. Needed for modf.
    double phase = modf(computer.calcPulsePhase(*abs_evt_time) + user_phi0, &int_part);

    // Write phase into output column.
    (*itor)["PULSE_PHASE"].set(phase);
  }

  // Clean up.
  delete events;
}

st_app::StAppFactory<PulsePhaseApp> g_factory("pulsePhase");
