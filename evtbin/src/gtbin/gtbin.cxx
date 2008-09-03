/** \file EvtBin.cxx
    \brief Event binning executable. Really this is a shell application whose run() method creates and
    runs one of several specialized binning applications.

    Some explanations of the classes. The evtbin application behaves like a number of similar tasks.
    For example, it can create light curves as well as single and multiple spectra. Each one of these tasks
    could itself be a separate application, albeit with similar input parameters and algorithms. Therefore, for
    each specific task behavior of evtbin, there is a specific application class: LightCurveApp, SingleSpectrumApp,
    etc. However, these have a great deal in common, so they derive from a common base class EvtBinAppBase,
    to reduce redundancy. (EvtBinAppBase in turn derives from st_app::StApp.) In addition, there is a master
    application class, EvtBin, which derives directly from st_app::StApp, but merely determines which of the other
    application objects is appropriate, instantiates and runs the application object. This master class, EvtBin,
    is the one which is used by the singleton st_app::StAppFactory to create the application.

    \author Yasushi Ikebe, GSSC
            James Peachey, HEASARC
*/
#include <cctype>
#include <memory>
#include <stdexcept>
#include <string>

// Helper class for creating binners based on standard parameter values.
#include "evtbin/BinConfig.h"

// Binners used:
#include "evtbin/LinearBinner.h"
#include "evtbin/LogBinner.h"

// Data product support classes.
#include "evtbin/CountCube.h"
#include "evtbin/CountMap.h"
#include "evtbin/DataProduct.h"
#include "evtbin/LightCurve.h"
#include "evtbin/MultiSpec.h"
#include "evtbin/SingleSpec.h"

// Hoops exceptions.
#include "hoops/hoops_exception.h"

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

// Identify cvs version tag.
const std::string s_cvs_id("$Name$");

/** \class EvtBinAppBase
    \brief Base class for specific binning applications. This has a generic run() method which is valid for
    all binning applications. The logic of run() is a good place to start to understand how the pieces fit together.
*/
class EvtBinAppBase : public st_app::StApp {
  public:
    /** \brief Construct a binning application with the given name.
        \param app_name the name of the application.
    */
    EvtBinAppBase(const std::string & app_name): m_bin_config(0), m_app_name(app_name) {}

    virtual ~EvtBinAppBase() throw() { delete m_bin_config; }

    /** \brief Standard "main" for an event binning application. This is the standard recipe for binning,
        with steps which vary between specific apps left to subclasses to define.
    */
    virtual void run() {
      using namespace evtbin;

      // Get parameter file object.
      st_app::AppParGroup & pars = getParGroup(m_app_name);

      // Prompt for input event file and extension names.
      pars.Prompt("evfile");
      pars.Prompt("evtable");

      // Create bin configuration object.
      m_bin_config = BinConfig::create(pars["evfile"]);

      // Prompt for parameters necessary for this application. This will probably be overridden in subclasses.
      parPrompt(pars);

      // Save all parameters from this tool run now.
      pars.Save();

      // Get data product. This is definitely overridden in subclasses to produce the correct type product
      // for the specific application.
      std::auto_ptr<DataProduct> product(createDataProduct(pars));

      // Bin input data into product.
      product->binInput();

      // Write the data product output.
      product->writeOutput(m_app_name, pars["outfile"]);
    }

    /** \brief Prompt for all parameters needed by a particular binner. The base class version prompts
        for universally needed parameters.
        \param pars The parameter prompting object.
    */
    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Prompt for input event file and output outfile. All binners need these.
      pars.Prompt("outfile");
      pars.Prompt("scfile");
      pars.Prompt("sctable");
    }

    /** \brief Create a specific data product object using the given parameters.
        \param pars The parameter prompting object.
    */
    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) = 0;

  protected:
    std::string getScFileName(const std::string & sc_file) const {
      std::string real_sc_file = sc_file;
      // Convert whole string to lowercase for purposes of comparison to special string "none"..
      for (std::string::iterator itor = real_sc_file.begin(); itor != real_sc_file.end(); ++itor) *itor = tolower(*itor);
      // Replace "none" with blank, and otherwise use original file name.
      if (real_sc_file == "none") real_sc_file = "";
      else real_sc_file = sc_file;

      return real_sc_file;
    }

    evtbin::BinConfig * m_bin_config;

  private:
    std::string m_app_name;
};

/** \class CountCubeApp
    \brief Count Cube specific binning application.
*/
class CountCubeApp : public EvtBinAppBase {
  public:
    CountCubeApp(const std::string & app_name): EvtBinAppBase(app_name) {}

    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Call base class prompter for standard universal parameters.
      EvtBinAppBase::parPrompt(pars);

      // Call configuration object to prompt for spatial binning related parameters.
      m_bin_config->spatialParPrompt(pars);

      // Call configuration object to prompt for spatial binning related parameters.
      m_bin_config->energyParPrompt(pars);
    }

    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      unsigned long num_x_pix = 0;
      unsigned long num_y_pix = 0;

      // Hoops throws an exception if one tries to convert a signed to an unsigned parameter value.
      try {
        // The conversion will work even if the exception is thrown.
        pars["nxpix"].To(num_x_pix);
      } catch (const hoops::Hexception & x) {
        // Ignore just the "signedness" error.
        if (hoops::P_SIGNEDNESS != x.Code()) throw;
      }

      // Hoops throws an exception if one tries to convert a signed to an unsigned parameter value.
      try {
        // The conversion will work even if the exception is thrown.
        pars["nypix"].To(num_y_pix);
      } catch (const hoops::Hexception & x) {
        // Ignore just the "signedness" error.
        if (hoops::P_SIGNEDNESS != x.Code()) throw;
      }

      // Create configuration-specific GTI.
      std::auto_ptr<Gti>gti(m_bin_config->createGti(pars));

      // Get the coordsys parameter and use it to determine what type coordinate system to use.
      bool use_lb = false;
      std::string coord_sys = pars["coordsys"];
      for (std::string::iterator itor = coord_sys.begin(); itor != coord_sys.end(); ++itor) *itor = tolower(*itor);
      if (coord_sys == "cel") use_lb = false;
      else if (coord_sys == "gal") use_lb = true;
      else throw std::logic_error(
        "CountCubeApp::createDataProduct does not understand \"" + pars["coordsys"].Value() + "\" coordinates");

      // Get binner for energy from energy application object.
      std::auto_ptr<Binner> energy_binner(m_bin_config->createEnergyBinner(pars));

      // Get a binner for energy bounds.
      std::auto_ptr<Binner> ebounds(m_bin_config->createEbounds(pars));

      return new evtbin::CountCube(pars["evfile"], pars["evtable"], getScFileName(pars["scfile"]), pars["sctable"],
        pars["xref"], pars["yref"], pars["proj"], num_x_pix, num_y_pix, pars["binsz"], pars["axisrot"],
        use_lb, pars["rafield"], pars["decfield"], *energy_binner, *ebounds, *gti);
    }
};

/** \class CountMapApp
    \brief Count Map specific binning application.
*/
class CountMapApp : public EvtBinAppBase {
  public:
    CountMapApp(const std::string & app_name): EvtBinAppBase(app_name) {}

    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Call base class prompter for standard universal parameters.
      EvtBinAppBase::parPrompt(pars);

      // Call configuration object to prompt for spatial binning related parameters.
      m_bin_config->spatialParPrompt(pars);
    }

    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      unsigned long num_x_pix = 0;
      unsigned long num_y_pix = 0;

      // Hoops throws an exception if one tries to convert a signed to an unsigned parameter value.
      try {
        // The conversion will work even if the exception is thrown.
        pars["nxpix"].To(num_x_pix);
      } catch (const hoops::Hexception & x) {
        // Ignore just the "signedness" error.
        if (hoops::P_SIGNEDNESS != x.Code()) throw;
      }

      // Hoops throws an exception if one tries to convert a signed to an unsigned parameter value.
      try {
        // The conversion will work even if the exception is thrown.
        pars["nypix"].To(num_y_pix);
      } catch (const hoops::Hexception & x) {
        // Ignore just the "signedness" error.
        if (hoops::P_SIGNEDNESS != x.Code()) throw;
      }

      // Create configuration-specific GTI.
      std::auto_ptr<Gti>gti(m_bin_config->createGti(pars));

      // Get the coordsys parameter and use it to determine what type coordinate system to use.
      bool use_lb = false;
      std::string coord_sys = pars["coordsys"];
      for (std::string::iterator itor = coord_sys.begin(); itor != coord_sys.end(); ++itor) *itor = tolower(*itor);
      if (coord_sys == "cel") use_lb = false;
      else if (coord_sys == "gal") use_lb = true;
      else throw std::logic_error(
        "CountMapApp::createDataProduct does not understand \"" + pars["coordsys"].Value() + "\" coordinates");

      return new evtbin::CountMap(pars["evfile"], pars["evtable"], getScFileName(pars["scfile"]), pars["sctable"],
        pars["xref"], pars["yref"], pars["proj"], num_x_pix, num_y_pix, pars["binsz"], pars["axisrot"],
        use_lb, pars["rafield"], pars["decfield"], *gti);
    }
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

      // Call time binner to prompt for time binning related parameters.
      m_bin_config->timeParPrompt(pars);
    }

    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      // Create configuration-specific time binner.
      std::auto_ptr<Binner> binner(m_bin_config->createTimeBinner(pars));

      // Create configuration-specific GTI.
      std::auto_ptr<Gti>gti(m_bin_config->createGti(pars));

      // Create data object from Binner.
      return new LightCurve(pars["evfile"], pars["evtable"], getScFileName(pars["scfile"]), pars["sctable"], *binner, *gti);
    }
};

/** \class SingleSpectrumApp
    \brief Single spectrum-specific binning application.
*/
class SingleSpectrumApp : public EvtBinAppBase {
  public:
    SingleSpectrumApp(const std::string & app_name): EvtBinAppBase(app_name) {}

    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Call base class prompter for standard universal parameters.
      EvtBinAppBase::parPrompt(pars);

      // Call energy binner to prompt for energy binning related parameters.
      m_bin_config->energyParPrompt(pars);
    }

    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars);
};

evtbin::DataProduct * SingleSpectrumApp::createDataProduct(const st_app::AppParGroup & pars) {
  using namespace evtbin;

  // Create binner.
  std::auto_ptr<Binner> binner(m_bin_config->createEnergyBinner(pars));

  // Create ebounds binner.
  std::auto_ptr<Binner> ebounds(m_bin_config->createEbounds(pars));

  // Create configuration-specific GTI.
  std::auto_ptr<Gti>gti(m_bin_config->createGti(pars));

  // Create data product.
  return new SingleSpec(pars["evfile"], pars["evtable"], getScFileName(pars["scfile"]), pars["sctable"], *binner, *ebounds, *gti);
}

/** \class MultiSpectraApp
    \brief Multiple spectra-specific binning application.
*/
class MultiSpectraApp : public EvtBinAppBase {
  public:
    MultiSpectraApp(const std::string & app_name): EvtBinAppBase(app_name) {}

    virtual void parPrompt(st_app::AppParGroup & pars) {
      // Call base class prompter for standard universal parameters.
      EvtBinAppBase::parPrompt(pars);

      // Use configuration object to prompt for energy binning related parameters.
      m_bin_config->energyParPrompt(pars);

      // Use configuration object to prompt for time binning related parameters.
      m_bin_config->timeParPrompt(pars);
    }

    virtual evtbin::DataProduct * createDataProduct(const st_app::AppParGroup & pars) {
      using namespace evtbin;

      // Get binner for time from time bin configuration object.
      std::auto_ptr<Binner> time_binner(m_bin_config->createTimeBinner(pars));

      // Get binner for energy from energy application object.
      std::auto_ptr<Binner> energy_binner(m_bin_config->createEnergyBinner(pars));

      // Create ebounds binner.
      std::auto_ptr<Binner> ebounds(m_bin_config->createEbounds(pars));

      // Create configuration-specific GTI.
      std::auto_ptr<Gti>gti(m_bin_config->createGti(pars));

      // Create data product.
      return new MultiSpec(pars["evfile"], pars["evtable"], getScFileName(pars["scfile"]), pars["sctable"], *time_binner,
        *energy_binner, *ebounds, *gti);
    }
};

/** \class GtBinApp
    \brief Application singleton for evtbin. Main application object, which just determines which
    of the several tasks the user wishes to perform, and creates and runs a specific application to perform
    this task.
*/
class GtBinApp : public st_app::StApp {
  public:
    GtBinApp(): st_app::StApp() {
      setName("gtbin");
      setVersion(s_cvs_id);

      // Get parameter file object.
      st_app::AppParGroup & pars = getParGroup("gtbin");

      // Set up logic for prompts/GUI layout.
#if 0
      pars.setSwitch("algorithm");
      pars.setCase("algorithm", "CMAP", "nxpix");
      pars.setCase("algorithm", "CMAP", "nypix");
      pars.setCase("algorithm", "CMAP", "binsz");
      pars.setCase("algorithm", "CMAP", "coordsys");
      pars.setCase("algorithm", "CMAP", "xref");
      pars.setCase("algorithm", "CMAP", "yref");
      pars.setCase("algorithm", "CMAP", "axisrot");
      pars.setCase("algorithm", "CMAP", "rafield");
      pars.setCase("algorithm", "CMAP", "decfield");
      pars.setCase("algorithm", "CMAP", "proj");

      pars.setCase("algorithm", "LC", "tbinalg");
      pars.setCase("algorithm", "PHA2", "tbinalg");
#endif

      pars.setSwitch("tbinalg");
      pars.setCase("tbinalg", "FILE", "tbinfile");
      pars.setCase("tbinalg", "LIN", "tstart");
      pars.setCase("tbinalg", "LIN", "tstop");
      pars.setCase("tbinalg", "LIN", "dtime");
      pars.setCase("tbinalg", "SNR", "snratio");
      pars.setCase("tbinalg", "SNR", "lcemin");
      pars.setCase("tbinalg", "SNR", "lcemax");
#if 0
      pars.setCase("algorithm", "LC", "tfield");

      pars.setCase("algorithm", "PHA1", "ebinalg");
      pars.setCase("algorithm", "PHA2", "ebinalg");
#endif

      pars.setSwitch("ebinalg");
      pars.setCase("ebinalg", "FILE", "ebinfile");
      pars.setCase("ebinalg", "LIN", "emin");
      pars.setCase("ebinalg", "LIN", "emax");
      pars.setCase("ebinalg", "LIN", "denergy");
      pars.setCase("ebinalg", "LOG", "emin");
      pars.setCase("ebinalg", "LOG", "emax");
      pars.setCase("ebinalg", "LOG", "enumbins");

#if 0
      pars.setCase("algorithm", "PHA1", "efield");
      pars.setCase("algorithm", "PHA2", "efield");
#endif

    }

    /** \brief Perform the action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      using namespace std;

      // Get parameter file object.
      st_app::AppParGroup & pars = getParGroup("gtbin");

      // Load standard mission/instrument bin configurations.
      evtbin::BinConfig::load();

      // Prompt for algorithm parameter, which determines which application is really used.
      pars.Prompt("algorithm");

      std::string algorithm = pars["algorithm"];

      // Make all upper case for case-insensitive comparisons.
      for (std::string::iterator itor = algorithm.begin(); itor != algorithm.end(); ++itor) *itor = toupper(*itor);

      // Based on this parameter, create the real application.
      std::auto_ptr<st_app::StApp> app(0);

      if (0 == algorithm.compare("CCUBE")) app.reset(new CountCubeApp("gtbin"));
      else if (0 == algorithm.compare("CMAP")) app.reset(new CountMapApp("gtbin"));
      else if (0 == algorithm.compare("LC")) app.reset(new LightCurveApp("gtbin"));
      else if (0 == algorithm.compare("PHA1")) app.reset(new SingleSpectrumApp("gtbin"));
      else if (0 == algorithm.compare("PHA2")) app.reset(new MultiSpectraApp("gtbin"));
      else throw std::logic_error(std::string("Algorithm ") + pars["algorithm"].Value() + " is not supported");

      // Pass on all parameter settings to the real app. (Needed for unlearned parameters.)
      st_app::AppParGroup & app_pars(app->getParGroup("gtbin"));
      app_pars = pars;

      // Prompt mode is set during construction, so set it explicitly.
      app_pars.setPromptMode(pars.getPromptMode());

      // Run the real application.
      app->run();
    }

};

// TODO for light curve:
// 3. Need different template which has no TIME_DEL column in uniform case.

// TODO for Spectra
// 1. Energy units: Xspec needs keV, input is MeV but could be anything (read keyword)

/// \brief Create factory object which can create the application:
st_app::StAppFactory<GtBinApp> g_app_factory("gtbin");
