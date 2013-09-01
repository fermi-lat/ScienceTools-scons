#include <string>

#include "pulsePhase/TimingModel.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

class PulsePhaseApp : public st_app::StApp {
  public:
    virtual void run();
};

void PulsePhaseApp::run() {
  st_app::AppParGroup & par_group = getParGroup("pulsePhase"); // getParGroup is in base class st_app::StApp

  par_group.Prompt(); // Prompts for all parameters.

  par_group.Save(); // Save the values of the parameters.

  // Open the event file.
  tip::Table * events = tip::IFileSvc::instance().editTable(par_group["eventfile"], "EVENTS");
  
  // Get model parameters.
  double epoch = par_group["epoch"];
  double phi0 = par_group["phi0"];
  double f0 = par_group["f0"];
  double f1 = par_group["f1"];
  double f2 = par_group["f2"];
  std::string time_field = par_group["timefield"];

  // Create the model.
  TimingModel model(epoch, phi0, f0, f1, f2);

  // Iterate over events.
  for (tip::Table::Iterator itor = events->begin(); itor != events->end(); ++itor) {
    tip::Table::Record & rec = *itor;
    // Calculate phase.
    double phase = model.calcPhase(rec[time_field].get());

    // Write phase into output column.
    rec["PULSE_PHASE"].set(phase);
  }

  // Clean up.
  delete events;

}

st_app::StAppFactory<PulsePhaseApp> g_factory;
