#include <cstdlib>
#include <iostream>

#include "TimingModel.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

int main() {
  int status = 0;

  std::cerr.precision(16);
  std::cout.precision(16);

  // Get top directory.
  const char * root_dir = getenv("PULSEPHASEROOT");
  if (0 == root_dir) {
    std::cerr << "Set PULSEPHASEROOT before running the test." << std::endl;
    return 1;
  }

  // Set the data directory.
  std::string data_dir = std::string(root_dir) + "/data/";

  // Simple test of timing model computation.
  TimingModel model(123456781.234564, 0.15, 29.9256509592326, -3.76909e-10, 1.29e-20);

  double phase = model.calcPhase(987654321.456987);

  // Result determined independently.
  if (phase != .03754153627346701771) {
    status = 1;
    std::cerr << "ERROR: calc_phase produced phase == " << phase << " not .03754153627346701771" << std::endl;
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

  // Test FrequencyCoeff class:
  TimingModel model2(123456781.234564, 0.15, TimingModel::FrequencyCoeff(29.9256509592326, -3.76909e-10, 1.29e-20));
  double phase2 = model2.calcPhase(987654321.456987);
  if (phase2 != phase) {
    status = 1;
    std::cerr << "ERROR: calc_phase using FrequencyCoeff produced phase == " << phase2 << " not " << phase << std::endl;
  }

  TimingModel::FrequencyCoeff fc(29.9256509592326, -3.76909e-10, 1.29e-20);
  TimingModel::PeriodCoeff pc = fc.invert();
  // std::cout << pc.m_term[0] << std::endl;
  // std::cout << pc.m_term[1] << std::endl;
  // std::cout << pc.m_term[2] << std::endl;

  // Test PeriodCoeff class:
  // TODO: (Masa) Get correct coefficients here to make this test meaningful.
  TimingModel model3(123456781.234564, 0.15, pc);
  double phase3 = model3.calcPhase(987654321.456987);
  if (phase2 != phase3) {
    status = 1;
    std::cerr << "ERROR: calc_phase using PeriodCoeff produced phase == " << phase3 << " not " << phase2 << std::endl;
  }
  
  delete events;

  return status;
}
