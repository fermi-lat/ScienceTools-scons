/** \file EvtBin.cxx
    \brief Event binning executable.
    \author Yasushi Ikebe, GSSC
            James Peachey, HEASARC
*/

#include <math.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

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
#include "evtbin/TipEqualLinearBinner.h"
#include "evtbin/TipEqualLogBinner.h"

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
      else if (0 == algorithm.compare("SPEC")) makeSimpleSpectrum(pars);
      else throw std::logic_error("Only Light Curve is currently supported");
    }

    void makeSimpleSpectrum(const st_app::AppParGroup & pars) {
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

      // Create binner object:
      TipEqualLogBinner binner("ENERGY", "CHANNEL", "COUNTS", energy_min, energy_max, (unsigned long)(enum_bins));

      // Fill the histogram:
      binner.fillBins(table->begin(), table->end());

      // Open output table for writing:
      tip::Table * out_table = tip::IFileSvc::instance().editTable(out_file, "SPECTRUM");

      // Write the output table:
      binner.writeHistogram(out_table);

// TODO:
// 1. Energy units: Xspec needs keV, input is MeV but could be anything (read keyword)
      delete out_table;

      delete table;
    }

    void makeLightCurve(const st_app::AppParGroup & pars) {
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

      // Create binner object:
      TipEqualLinearBinner binner("TIME", "COUNTS", tstart, tstop, width);

      // Fill the histogram:
      binner.fillBins(table->begin(), table->end());

      // Open output table for writing:
      tip::Table * out_table = tip::IFileSvc::instance().editTable(out_file, "RATE");

      // Write the output table:
      binner.writeHistogram(out_table);

// TODO for light curve:
// 1. Copy GTI from event file (anding with the range [tstart, tstop])
// 2. Get bin defs (non const) from timebinalg or timebinfile
// 3. Need different template which has no TIME_DEL column in uniform case.
// 4. Livetime (applies for all binners) from ft2 file gets written in header of histogram.
//    Recompute iff GTI changed (in step 1).
      delete out_table;

      delete table;
    }
};

/// \brief Create factory object which can create the application:
st_app::StAppFactory<EvtBin> g_app_factory;
