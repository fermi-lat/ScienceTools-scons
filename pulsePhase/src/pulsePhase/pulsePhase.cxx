#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

#include "pulsePhase/TimingModel.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

using namespace pulsePhase;

class PulsePhaseApp : public st_app::StApp {
  public:
    virtual void run();
};

void PulsePhaseApp::run() {
  st_app::AppParGroup & par_group = getParGroup("pulsePhase"); // getParGroup is in base class st_app::StApp

  // par_group.Prompt(); // Prompts for all parameters.
  par_group.Prompt("eventfile");
  par_group.Save();

  par_group.Prompt("ephstyle");
  std::string eph_style = par_group["ephstyle"];
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
  } else {
    throw std::runtime_error("Ephemeris style \"" + eph_style + "\" is not supported.");
  }

  par_group.Save(); // Save the values of the parameters.

  // Open the event file.
  tip::Table * events = tip::IFileSvc::instance().editTable(par_group["eventfile"], "EVENTS");

  // Get model parameters.
  double epoch = par_group["epoch"];
  double users_phi0 = par_group["phi0"];
  TimingModel * model = 0;

  if (eph_style == "FREQ") {
    double f0 = par_group["f0"];
    double f1 = par_group["f1"];
    double f2 = par_group["f2"];
    TimingModel::FrequencyCoeff coeff(f0, f1, f2);
    model = new TimingModel(epoch, 0., coeff);
  } else if (eph_style == "PER") {
    double p0 = par_group["p0"];
    double p1 = par_group["p1"];
    double p2 = par_group["p2"];
    TimingModel::PeriodCoeff coeff(p0, p1, p2);
    model = new TimingModel(epoch, 0., coeff);
  } else {
    throw std::runtime_error("Ephemeris style \"" + eph_style + "\" is not supported.");
  }
  std::string time_field = par_group["timefield"];

  // Iterate over events.
  for (tip::Table::Iterator itor = events->begin(); itor != events->end(); ++itor) {
    tip::Table::Record & rec = *itor;
    // Calculate phase.
    double int_part; // Ignored. Needed for modf.
    double phase = modf(model->calcPhase(rec[time_field].get()) + users_phi0, &int_part);

    // Write phase into output column.
    rec["PULSE_PHASE"].set(phase);
  }

  // Clean up.
  delete model;
  delete events;
}

st_app::StAppFactory<PulsePhaseApp> g_factory;
