/** \file RspGenApp.cxx
    \brief Implementation of main rspgen application class.
    \author James Peachey, HEASARC
*/
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>

#include "irfLoader/Loader.h"

#include "RspGenApp.h"
#include "rspgen/GrbResponse.h"
#include "rspgen/IResponse.h"
#include "rspgen/PointResponse.h"

#include "tip/Header.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/Env.h"

static std::string s_cvs_id = "$Name$";

namespace rspgen {

  RspGenApp::RspGenApp(): m_bin_config(0), m_data_dir(), m_response(0) {
    setName("gtrspgen");
    setVersion(s_cvs_id);
  }

  RspGenApp::~RspGenApp() throw() { delete m_response; delete m_bin_config; }

  void RspGenApp::run() {
    st_app::AppParGroup & pars(getParGroup("gtrspgen"));
    loadResponses();
    evtbin::BinConfig::load();
    prompt(pars);
    writeResponse(pars);
  }

  void RspGenApp::prompt(st_app::AppParGroup & pars) {
    // First determine which special case to handle.
    pars.Prompt("respalg");
    std::string alg = pars["respalg"];
    if (alg == "GRB");
    else if (alg == "PS");
    else throw std::runtime_error("RspGenApp::prompt(): unknown/invalid response algorithm " + alg);
    
    pars.Prompt("specfile");
    pars.Prompt("scfile");
    pars.Prompt("outfile");
    pars.Prompt("ra");
    pars.Prompt("dec");

    // In the burst case, the time of the burst is used to find a single RA/DEC.
    if (alg == "GRB") pars.Prompt("time");
    else {
      // Non-burst case: SC data will be binned in theta space.
      pars.Prompt("thetacut");
      pars.Prompt("thetabinsize");
    }

    // Prompt for remaining parameters, which are common to all.
    pars.Prompt("psfradius");
    pars.Prompt("resptype");
    pars.Prompt("resptpl");

    // Prompt for (true) energy binning parameters.
    getConfig(pars)->energyParPrompt(pars);

    pars.Save();
  }

  void RspGenApp::writeResponse(const st_app::AppParGroup & pars) {
    // Create energy binner from related parameters.
    std::auto_ptr<evtbin::Binner> true_en_binner(getConfig(pars)->createEnergyBinner(pars));

    // Get name of template for output file.
    std::string resp_tpl = pars["resptpl"];

    // If it was not defined, get default template file name.
    if (0 == resp_tpl.compare("DEFAULT")) resp_tpl = st_facilities::Env::appendFileName(getDataDir(), "LatResponseTemplate");

    // Determine which algorithm to use.
    std::string alg = pars["respalg"];

    // Clean up any previous response.
    delete m_response; m_response = 0;

    // Extract name of output file.
    std::string out_file = pars["outfile"];

    // Adjust keywords in spectrum.
    try {
      std::auto_ptr<tip::Table> spectrum(tip::IFileSvc::instance().editTable(pars["specfile"], "SPECTRUM"));
      spectrum->getHeader()["RESPFILE"].set(out_file);
    } catch (const tip::TipException &) {
      // If it can't be written, e.g. because the spectrum is write protected, just issue a warning.
      // TODO: add warning using st_stream.
    }

    // Create response object.
    if (alg == "GRB") {
      m_response = new GrbResponse(pars["ra"], pars["dec"], pars["time"], pars["psfradius"],
        pars["resptype"], pars["specfile"], pars["scfile"], true_en_binner.get());
    } else if (alg == "PS") {
      m_response = new PointResponse(pars["ra"], pars["dec"], pars["thetacut"], pars["thetabinsize"], pars["psfradius"],
        pars["resptype"], pars["specfile"], pars["scfile"], true_en_binner.get());
    } else {
      throw std::runtime_error("RspGenApp::writeResponse: invalid response algorithm " + alg);
    }

    // Write the output response file.
    m_response->writeOutput("gtrspgen", out_file, resp_tpl);
  }

  void RspGenApp::loadResponses() {
    irfLoader::Loader::go();
  }

  std::string RspGenApp::getDataDir() const {
    static std::string retval = st_facilities::Env::getDataDir("rspgen");
    return retval;
  }

  evtbin::BinConfig * RspGenApp::getConfig(const st_app::AppParGroup & pars) {
    if (0 == m_bin_config)
      m_bin_config = evtbin::BinConfig::create(pars["specfile"]);
    return m_bin_config;
  }

}
