#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "pulsePhase/TimingModel.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

using namespace pulsePhase;

int main() {
  int status = 0;

  std::cerr.precision(20);
  std::cout.precision(20);

  // Get top directory.
  const char * root_dir = getenv("PULSEPHASEROOT");
  if (0 == root_dir) {
    std::cerr << "Set PULSEPHASEROOT before running the test." << std::endl;
    return 1;
  }

  // Set the data directory.
  std::string data_dir = std::string(root_dir) + "/data/";

  // Simple test of timing model computation.
  TimingModel model(123.456789, .11, 1.125e-2, -2.25e-4, 6.75e-6);

  double phase = model.calcPhase(223.456789);

  double epsilon = 1.e-8;

  // Result determined independently.
  if (fabs(phase/.235 - 1.) > epsilon) {
    status = 1;
    std::cerr << "ERROR: calc_phase produced phase == " << phase << " not .235" << std::endl;
  }

  // Test FrequencyCoeff class:
  TimingModel model2(123.456789, 0.11, TimingModel::FrequencyCoeff(1.125e-2, -2.25e-4, 6.75e-6));
  double phase2 = model2.calcPhase(223.456789);
  if (fabs(phase2/phase - 1.) > epsilon) {
    status = 1;
    std::cerr << "ERROR: calc_phase using FrequencyCoeff produced phase == " << phase2 << " not " << phase << std::endl;
  }

  // Test PeriodCoeff class:
  // This is a set of values known to be the inverses of the frequency coefficients above.
  TimingModel::PeriodCoeff pc(88.8888888888888888888889, 1.777777777777777777777778, 0.0177777777777777777777778);

  TimingModel model3(123.456789, 0.11, pc);
  double phase3 = model3.calcPhase(223.456789);
  if (fabs(phase3/phase - 1.) > epsilon) {
    status = 1;
    std::cerr << "ERROR: calc_phase using PeriodCoeff produced phase == " << phase3 << " not " << phase2 << std::endl;
  }
 
  // Next test: read event file.
  tip::Table * events = tip::IFileSvc::instance().editTable(data_dir + "D1.fits", "EVENTS");
 
  // Iterate over events.
  for (tip::Table::Iterator itor = events->begin(); itor != events->end(); ++itor) {
    tip::Table::Record & rec = *itor;
    // Calculate phase.
    double phase = model.calcPhase(rec["TIME"].get());

    // Write phase into output column.
    rec["PULSE_PHASE"].set(phase);
  }

  delete events;

  // Test calcPdotCorr method.
  // Change coefficients to produce a visible effect.
  TimingModel model4(123.4567891234567, .11, 1.125e-2, -2.25e-4, 13.5e-6);
  double evt_time = 223.4567891234567;
  double pdot_t = model4.calcPdotCorr(evt_time);
  double correct_t = 323.4567891234567;
  // For this test, time difference between these two values must be << 1.e-6.
  if (fabs(pdot_t - correct_t) > epsilon) {
    status = 1;
    std::cerr << "ERROR: calcPdotCorr produced pdot-corrected time == " << pdot_t << " not " << correct_t << std::endl;
  }
  
  return status;
}
