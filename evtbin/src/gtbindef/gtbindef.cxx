#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_facilities/Env.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

class BinMakerApp : public st_app::StApp {
  public:
    virtual void run();

    virtual void prompt(st_app::AppParGroup & pars);
};

void BinMakerApp::run() {
  // Get parameters.
  st_app::AppParGroup & pars(getParGroup("gtbindef"));

  prompt(pars);

  std::string in_file = pars["binfile"];
  std::string bin_type = pars["bintype"];
  std::string energy_units = pars["energyunits"];
  std::string out_file = pars["outfile"];

  std::string data_dir = st_facilities::Env::getDataDir("evtbin");

  // Determine whether writing time or energy bins.
  std::string start_field;
  std::string stop_field;
  std::string table_name;
  std::string template_file;
  double unit_multiplier = 1.;
  if (bin_type == "E") {
    start_field = "E_MIN";
    stop_field = "E_MAX";
    table_name = "ENERGYBINS";
    template_file = st_facilities::Env::appendFileName(data_dir, "LatEnergyBinDef.tpl");

    // Determine multiplier for output energy units.
    if (energy_units == "GeV") unit_multiplier = 1.e6;
    else if (energy_units == "MeV") unit_multiplier = 1.e3;
    else if (energy_units == "keV") unit_multiplier = 1.;
    else if (energy_units == "eV") unit_multiplier = 1.e-3;
    else throw std::runtime_error("BinMakerApp::run does not accept energy bins specified in units of " + energy_units);
  } else {
    start_field = "START";
    stop_field = "STOP";
    table_name = "TIMEBINS";
    template_file = st_facilities::Env::appendFileName(data_dir, "LatTimeBinDef.tpl");
  }

  std::ifstream fs(in_file.c_str());
  fs.precision(24);
  if (!fs) throw std::runtime_error("BinMakerApp::run could not open file " + in_file);

  // Create output file.
  using namespace tip;
  IFileSvc & file_svc(IFileSvc::instance());

  file_svc.createFile(out_file, template_file);

  // Populate output table.
  std::auto_ptr<Table> out_table(file_svc.editTable(out_file, table_name));

  Table::Iterator itor = out_table->begin();
  while (fs) {
    // Read input start and stop values for bins.
    double start = 0.;
    fs >> start;
    if (!fs) continue;
    double stop = start;
    fs >> stop;
    if (!fs) throw std::runtime_error("BinMakerApp::run: incomplete row in input bin specification file.");

    // Handle unit conversions.
    if (unit_multiplier != 1.) {
      start *= unit_multiplier;
      stop *= unit_multiplier;
    }

    // Write output table values.
    (*itor)[start_field].set(start);
    (*itor)[stop_field].set(stop);

    ++itor;
  }
}

void BinMakerApp::prompt(st_app::AppParGroup & pars) {
  pars.Prompt("bintype");
  pars.Prompt("binfile");
  pars.Prompt("outfile");

  if (pars["bintype"].Value() == "E") pars.Prompt("energyunits");

  pars.Save();
}

st_app::StAppFactory<BinMakerApp> g_factory;
