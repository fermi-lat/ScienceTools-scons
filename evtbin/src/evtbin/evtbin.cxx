/** \file EvtBin.cxx
    \brief Event binning executable.
    \author Yasushi Ikebe, GSSC
            James Peachey, HEASARC
*/

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

// Parameter file access from st_app.
#include "st_app/AppParGroup.h"
// Application base class.
#include "st_app/StApp.h"
// Factory used by st_app's standard main to create application object.
#include "st_app/StAppFactory.h"

// File access from tip.
#include "tip/IFileSvc.h"
// Table access from tip.
#include "tip/Table.h"

// Binners used:
#include "evtbin/Hist1D.h"
#include "evtbin/Hist2D.h"
#include "evtbin/LinearBinner.h"
#include "evtbin/LogBinner.h"
#include "evtbin/RecordBinFiller.h"

/** \class EvtBin
    \brief Application singleton for evtbin.
*/
class EvtBin : public st_app::StApp {
  public:
    /** \brief Perform the action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      using namespace std;

      // Get parameter file object:
      st_app::AppParGroup & pars = getParGroup("evtbin");

      // Prompt for all parameters:
      pars.Prompt();

      // Save values of parameters:
      pars.Save();

      std::string algorithm = pars["algorithm"];
      if (0 == algorithm.compare("LC")) makeLightCurve(pars);
      else if (0 == algorithm.compare("PHA2")) makePha2(pars);
      else if (0 == algorithm.compare("SPEC")) makeSimpleSpectrum(pars);
      else throw std::logic_error(std::string("Algorithm ") + algorithm + " is not supported");
    }

    void makeSimpleSpectrum(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      std::string event_file = pars["eventfile"];
      std::string out_file = pars["outfile"];
      
      // Get the IFileSvc singleton and use it to create the output file:
      // tip == namespace, IFileSvc == class, instance() is static method returning the singleton
      // createFile is a normal method:
      tip::IFileSvc::instance().createFile(out_file, "LatSingleBinnedTemplate");
 
      // Open input file for reading only:
      const tip::Table * table = tip::IFileSvc::instance().readTable(event_file, "EVENTS");

      // Now get ranges:
      double energy_min = pars["emin"];
      double energy_max = pars["emax"];
      long enum_bins = pars["enumbins"];

      // Make sure a positive number of bins was entered.
      if (0 >= enum_bins) throw std::runtime_error("Number of bins (enumbins parameter) must be positive");

      // Create histogram:
      Hist1D hist(LogBinner(energy_min, energy_max, enum_bins, "ENERGY"));

      // Fill the histogram:
      std::for_each(table->begin(), table->end(), RecordBinFiller(hist));

      // Open output table for writing:
      tip::Table * out_table = tip::IFileSvc::instance().editTable(out_file, "SPECTRUM");

      // Get the histogram's binner.
      const Binner * binner = hist.getBinners()[0];

      // From the binner, get the number of bins.
      long num_bins = binner->getNumBins();

      // Set size of output table such that it can accomodate these bins.
      out_table->setNumRecords(num_bins);

      // Write the output table.
      tip::Table::Iterator table_itor = out_table->begin();
      for (long index = 0; index != num_bins; ++index, ++table_itor) {
        (*table_itor)["CHANNEL"].set(index + 1);
        (*table_itor)["COUNTS"].set(hist[index]);
      }

// TODO:
// 1. Energy units: Xspec needs keV, input is MeV but could be anything (read keyword)
      delete out_table;

      delete table;
    }

    void makeLightCurve(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      std::string event_file = pars["eventfile"];
      std::string out_file = pars["outfile"];

      // Get the IFileSvc singleton and use it to create the output file:
      // tip == namespace, IFileSvc == class, instance() is static method returning the singleton
      // createFile is a normal method:
      tip::IFileSvc::instance().createFile(out_file, "LatLightCurveTemplate");
 
      // Open input file for reading only:
      const tip::Table * table = tip::IFileSvc::instance().readTable(event_file, "EVENTS");

      // Now get ranges:
      double tstart = pars["tstart"];
      double tstop = pars["tstop"];
      double width = pars["deltatime"];

      // Create histogram:
// TODO LinearBinner needs another constructor with the width specified, and needs to use this width...
      Hist1D hist(LinearBinner(tstart, tstop, long((tstop - tstart)/width), "TIME"));

      // Fill the histogram:
      std::for_each(table->begin(), table->end(), RecordBinFiller(hist));

      // Open output table for writing:
      tip::Table * out_table = tip::IFileSvc::instance().editTable(out_file, "RATE");

      // Get the histogram's binner.
      const Binner * binner = hist.getBinners()[0];

      // From the binner, get the number of bins.
      long num_bins = binner->getNumBins();

      // Set size of output table such that it can accomodate these bins.
      out_table->setNumRecords(num_bins);

      // Write the output table.
      tip::Table::Iterator table_itor = out_table->begin();
      for (long index = 0; index != num_bins; ++index, ++table_itor) {
        (*table_itor)["TIME"].set(binner->getInterval(index).midpoint());
        (*table_itor)["TIMEDEL"].set(binner->getInterval(index).width());
        (*table_itor)["COUNTS"].set(hist[index]);
      }

// TODO for light curve:
// 1. Copy GTI from event file (anding with the range [tstart, tstop])
// 2. Get bin defs (non const) from timebinalg or timebinfile
// 3. Need different template which has no TIME_DEL column in uniform case.
// 4. Livetime (applies for all binners) from ft2 file gets written in header of histogram.
//    Recompute iff GTI changed (in step 1).
      delete out_table;

      delete table;
    }

    void makePha2(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      std::string event_file = pars["eventfile"];
      std::string out_file = pars["outfile"];
      
      // Get the IFileSvc singleton and use it to create the output file:
      // tip == namespace, IFileSvc == class, instance() is static method returning the singleton
      // createFile is a normal method:
      tip::IFileSvc::instance().createFile(out_file, "LatSingleBinnedTemplate");
 
      // Open input file for reading only:
      const tip::Table * table = tip::IFileSvc::instance().readTable(event_file, "EVENTS");

      // Now get time ranges:
      double tstart = pars["tstart"];
      double tstop = pars["tstop"];
      double width = pars["deltatime"];

      // Now get energy ranges:
      double energy_min = pars["emin"];
      double energy_max = pars["emax"];
      long enum_bins = pars["enumbins"];

      // Make sure a positive number of bins was entered.
      if (0 >= enum_bins) throw std::runtime_error("Number of bins (enumbins parameter) must be positive");

      // Create one-dim binner objects:
//      TipEqualLinearBinner time_binner("TIME", "COUNTS", tstart, tstop, width);
//      TipEqualLogBinner energy_binner("ENERGY", "COUNTS", energy_min, energy_max, (unsigned long)(enum_bins));

      Hist2D hist(
        LinearBinner(tstart, tstop, long((tstop - tstart)/width), "TIME"),
        LogBinner(energy_min, energy_max, enum_bins, "ENERGY")
      );

      // Create two-dim gestalt binner which uses both the above binners:
//      TipBinner2D binner(&time_binner, &energy_binner);

      // Fill the histogram:
//      binner.fillBins(table->begin(), table->end());
      std::for_each(table->begin(), table->end(), RecordBinFiller(hist));

      // Open output table for writing:
      tip::Table * out_table = tip::IFileSvc::instance().editTable(out_file, "SPECTRUM");

      // Write the output table:
//      binner.writeHistogram(out_table);

      // Get the histogram's energy binner.
      const Binner * binner = hist.getBinners()[1];

      // From the binner, get the number of bins.
      long num_bins = binner->getNumBins();

      // Set size of output table such that it can accomodate these bins.
      out_table->setNumRecords(num_bins);

      // Write the output table.
      tip::Table::Iterator table_itor = out_table->begin();
      for (long index = 0; index != num_bins; ++index, ++table_itor) {
        (*table_itor)["CHANNEL"].set(index + 1);
        (*table_itor)["COUNTS"].set(hist[0][index]);
      }

// TODO:
// 1. Energy units: Xspec needs keV, input is MeV but could be anything (read keyword)
      delete out_table;

      delete table;
    }

};

/// \brief Create factory object which can create the application:
st_app::StAppFactory<EvtBin> g_app_factory;
