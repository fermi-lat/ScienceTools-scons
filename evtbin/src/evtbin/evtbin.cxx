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

// Use simple equal linear interval binner:
#include "evtbin/TipEqualLinearBinner.h"

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
      else throw std::logic_error("Only Light Curve is currently supported");
    }

    void makeLightCurve(const st_app::AppParGroup & pars) {
      std::string event_file = pars["eventfile"];
      std::string out_file = pars["outfile"];

      // Get the IFileSvc singleton and use it to create the output file:
      // tip == namespace, IFileSvc == class, instance() is static method returning the singleton
      // createFile is a normal method:
      tip::IFileSvc::instance().createFile(out_file, "LatLightCurveTemplate");
 
      // This is the same as:
      // tip::IFileSvc & the_service = tip::IFileSvc::instance();
      // the_service.createFile("LC1.lc", "LatLightCurveTemplate");

      // Open input file for reading only:
      const tip::Table * table = tip::IFileSvc::instance().readTable(event_file, "EVENTS");

      // Now make a functor to bin this vector:
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

      delete out_table;

      delete table;
    }
};

/// \brief Create factory object which can create the application:
st_app::StAppFactory<EvtBin> g_app_factory;
