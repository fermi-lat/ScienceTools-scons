/** \file EvtBin.cxx
    \brief Event binning executable. Really this is a shell application whose run() method creates and
    runs one of several specialized binning applications.

    Some explanations of the classes. The evtbin application behaves like a number of similar tasks.
    For example, it can create light curves as well as single and multiple spectra. Each one of these tasks
    could itself be a separate application, albeit with similar input parameters and algorithms. Therefore, for
    each specific task behavior of evtbin, there is a specific application class: LightCurveApp, SimpleSpectrumApp,
    etc. However, these have a great deal in common, so they derive from a common base class EvtBinAppBase,
    to reduce redundancy. (EvtBinAppBase in turn derives from st_app::StApp.) In addition, there is a master
    application class, EvtBin, which derives directly from st_app::StApp, but merely determines which of the other
    application objects is appropriate, instantiates and runs the application object. This master class, EvtBin,
    is the one which is used by the singleton st_app::StAppFactory to create the application.

    \author Yasushi Ikebe, GSSC
            James Peachey, HEASARC
*/
#include <stdexcept>
#include <string>

// Binners used:
#include "evtbin/LinearBinner.h"
#include "evtbin/LogBinner.h"

// Data product support classes.
#include "evtbin/DataProduct.h"
#include "evtbin/LightCurve.h"
#include "evtbin/MultiSpec.h"
#include "evtbin/SingleSpec.h"

// Interactive parameter file access from st_app.
#include "st_app/AppParGroup.h"
// Science Tools Application base class.
#include "st_app/StApp.h"
// Factory used by st_app's standard main to create application object.
#include "st_app/StAppFactory.h"

// File access from tip.
#include "tip/IFileSvc.h"
// Table access from tip.
#include "tip/Table.h"

/** \class EvtBinAppBase
    \brief Base class for specific binning applications. This has a generic run() method which is valid for
    all binning applications. The logic of run() is a good place to start to understand how the pieces fit together.
*/
class EvtBinAppBase : public st_app::StApp {
  public:
    /** \brief Construct a binning application with the given name.
        \param app_name the name of the application.
    */
    EvtBinAppBase(const std::string & app_name): m_app_name(app_name) {}

    virtual ~EvtBinAppBase() throw() {}

    /** \brief Standard "main" for an event binning application. This is the standard recipe for binning,
        with steps which vary between specific apps left to subclasses to define.
    */
    virtual void run() {
      using namespace evtbin;

      // Get parameter file object.
      st_app::AppParGroup & pars = getParGroup(m_app_name);

      // Prompt for parameters necessary for this application. This will probably be overridden in subclasses.
      parPrompt(pars);

      // Save all parameters from this tool run now.
      pars.Save();

      // Object to represent the input event file.
      const tip::Table * events = 0;

      // Object to represent the data product being produced.
      DataProduct * product = 0;

      try {
        // Open input file for reading only.
        events = tip::IFileSvc::instance().readTable(pars["eventfile"], "EVENTS");

        // Get data product. This is definitely overridden in subclasses to produce the correct type product
        // for the specific application.
        product = createDataProduct(pars);

        // Copy keywords from input events table.
        product->harvestKeywords(events->getHeader());

        // Bin input data into product.
        product->binInput(events->begin(), events->end());

        // Write the data product output.
        product->writeOutput(m_app_name, pars["outfile"]);

      } catch (...) {
        delete product;
        delete events;
        throw;
      }
      delete product;
      delete events;
    }

    /** \brief Prompt for all parameters needed by a particular binner. The base class version prompts
        for universally needed parameters.
        \param pars The parameter prompting object.
    */
    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Prompt for input eventfile and output outfile. All binners need these.
      pars.Prompt("eventfile");
      pars.Prompt("outfile");
    }

    /** \brief Create a specific data product object using the given parameters.
        \param pars The parameter prompting object.
    */
    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) = 0;

  private:
    std::string m_app_name;
};

/** \class LightCurveApp
    \brief Light curve specific binning application.
*/
class LightCurveApp : public EvtBinAppBase {
  public:
    LightCurveApp(const std::string & app_name): EvtBinAppBase(app_name) {}

    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Call base class prompter for standard universal parameters.
      EvtBinAppBase::parPrompt(pars);

      // Prompt for remaining parameters needed for product.
      pars.Prompt("tstart");
      pars.Prompt("tstop");
      pars.Prompt("deltatime");
    }

    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      // Get ranges:
      double tstart = pars["tstart"];
      double tstop = pars["tstop"];
      double width = pars["deltatime"];

      // Create data object.
      return new LightCurve(LinearBinner(tstart, tstop, long((tstop - tstart) / width), "TIME"));
    }
};

/** \class MultiSpectraApp
    \brief Multiple spectra-specific binning application.
*/
class MultiSpectraApp : public EvtBinAppBase {
  public:
    MultiSpectraApp(const std::string & app_name): EvtBinAppBase(app_name) {}

    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Call base class prompter for standard universal parameters.
      EvtBinAppBase::parPrompt(pars);

      // Prompt for remaining parameters needed for product.
      pars.Prompt("emin");
      pars.Prompt("emax");
      pars.Prompt("enumbins");

      pars.Prompt("tstart");
      pars.Prompt("tstop");
      pars.Prompt("deltatime");
    }

    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      // Get ranges:
      double energy_min = pars["emin"];
      double energy_max = pars["emax"];
      long energy_num_bins = pars["enumbins"];

      double tstart = pars["tstart"];
      double tstop = pars["tstop"];
      double width = pars["deltatime"];

      // Create data product.
      return new MultiSpec(LinearBinner(tstart, tstop, long((tstop - tstart)/width), "TIME"),
        LogBinner(energy_min, energy_max, energy_num_bins, "ENERGY"));
    }
};

/** \class SimpleSpectrumApp
    \brief Single spectrum-specific binning application.
*/
class SimpleSpectrumApp : public EvtBinAppBase {
  public:
    SimpleSpectrumApp(const std::string & app_name): EvtBinAppBase(app_name) {}

    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Call base class prompter for standard universal parameters.
      EvtBinAppBase::parPrompt(pars);

      // Prompt for remaining parameters needed for light curve.
      pars.Prompt("emin");
      pars.Prompt("emax");
      pars.Prompt("enumbins");
    }

    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      // Get ranges:
      double energy_min = pars["emin"];
      double energy_max = pars["emax"];
      long energy_num_bins = pars["enumbins"];

      // Create data product.
      return new SingleSpec(LogBinner(energy_min, energy_max, energy_num_bins, "ENERGY"));
    }
};

/** \class EvtBin
    \brief Application singleton for evtbin. Main application object, which just determines which
    of the several tasks the user wishes to perform, and creates and runs a specific application to perform
    this task.
*/
class EvtBin : public st_app::StApp {
  public:
    /** \brief Perform the action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      using namespace std;

      // Get parameter file object.
      st_app::AppParGroup & pars = getParGroup("evtbin");

      // Prompt for algorithm parameter, which determines which application is really used.
      pars.Prompt("algorithm");

      pars.Save();

      std::string algorithm = pars["algorithm"];

      // Based on this parameter, create the real application.
      st_app::StApp * app = 0;

      try {
        if (0 == algorithm.compare("LC")) app = new LightCurveApp("evtbin");
        else if (0 == algorithm.compare("PHA2")) app = new MultiSpectraApp("evtbin");
        else if (0 == algorithm.compare("SPEC")) app = new SimpleSpectrumApp("evtbin");
        else throw std::logic_error(std::string("Algorithm ") + algorithm + " is not supported");

        // Run the real application.
        app->run();
      } catch (...) {
        delete app;
        throw;
      }
      delete app;
    }

};

// TODO for light curve:
// 1. Copy GTI from event file (anding with the range [tstart, tstop])
// 2. Get bin defs (non const) from timebinalg or timebinfile
// 3. Need different template which has no TIME_DEL column in uniform case.
// 4. Livetime (applies for all binners) from ft2 file gets written in header of histogram.
//    Recompute iff GTI changed (in step 1).

// TODO for Spectra
// 1. Energy units: Xspec needs keV, input is MeV but could be anything (read keyword)

/// \brief Create factory object which can create the application:
st_app::StAppFactory<EvtBin> g_app_factory;
