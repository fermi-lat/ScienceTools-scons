/** \file EvtBin.cxx
    \brief Event binner.
    \author Yasushi Ikebe, GSSC
            James Peachey, HEASARC
*/

#include <algorithm>
#include <math.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

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

      // Get the IFileSvc singleton and use it to create the output file:
      // tip == namespace, IFileSvc == class, instance() is static method returning the singleton
      // createFile is a normal method:
      tip::IFileSvc::instance().createFile("LC1.lc", "LatLightCurveTemplate");
 
      // This is the same as:
      // tip::IFileSvc & the_service = tip::IFileSvc::instance();
      // the_service.createFile("LC1.lc", "LatLightCurveTemplate");

      // Open input file for reading only:
      const tip::Table * table = tip::IFileSvc::instance().readTable("D1.fits", "EVENTS");

      // Now make a functor to bin this vector:
      double tstart = 0.;
      double tstop = 900000.;
      double width = 1000.;

      // Create binner object:
      TipEqualLinearBinner binner("TIME", "COUNTS", tstart, tstop, width);

      // Fill the histogram:
      binner.fillBins(table->begin(), table->end());

      // Open output table for writing:
      tip::Table * out_table = tip::IFileSvc::instance().editTable("LC1.lc", "RATE");

      // Write the output table:
      binner.writeHistogram(out_table);

      delete out_table;

      delete table;
    }
};

/// \brief Create factory object which can create the application:
st_app::StAppFactory<EvtBin> g_app_factory;
