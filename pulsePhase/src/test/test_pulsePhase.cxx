#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

#include "pulsarDb/GlastTime.h"
#include "pulsarDb/PulsarEph.h"
#include "pulsarDb/TimingModel.h"

#include "st_facilities/Env.h"

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

using namespace pulsarDb;

int main() {
  int status = 0;

  std::cerr.precision(20);
  std::cout.precision(20);

  // Set the data directory.
  std::string data_dir;
  try {
    data_dir = st_facilities::Env::getDataDir("pulsePhase");
  } catch (const std::exception & x) {
    std::cerr << x.what() << std::endl;
    return 1;
  }

  // Simple test of timing model computation.
  TimingModel model;

  // Next test: read event file.
  std::auto_ptr<tip::Table> events(tip::IFileSvc::instance().editTable(st_facilities::Env::appendFileName(data_dir, "D1.fits"), "EVENTS"));
 
  double valid_since = 0.;
  double valid_until = 0.;

  const tip::Header & header(events->getHeader());
  header["TSTART"].get(valid_since);
  header["TSTOP"].get(valid_until);

  GlastTdbTime abs_valid_since(valid_since);
  FrequencyEph eph(abs_valid_since, GlastTdbTime(valid_until), GlastTdbTime(123.456789), .11, 1.125e-2, -2.25e-4, 6.75e-6);

  // Iterate over events.
  for (tip::Table::Iterator itor = events->begin(); itor != events->end(); ++itor) {
    tip::Table::Record & rec = *itor;
    // Calculate phase.
    double phase = model.calcPulsePhase(eph, GlastTdbTime(rec["TIME"].get()));

    // Write phase into output column.
    rec["PULSE_PHASE"].set(phase);
  }

  return status;
}
