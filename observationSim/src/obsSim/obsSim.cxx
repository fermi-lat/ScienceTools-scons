/**
 * @file obsSim.cxx
 * @brief A prototype O2 application.
 * @author J. Chiang
 *
 * $Header$
 */

#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <cassert>
#include <fstream>

#include "CLHEP/Random/Random.h"

#include "facilities/Util.h"

#include "astro/SkyDir.h"

#include "latResponse/Irfs.h"
#include "latResponse/IrfsFactory.h"

#include "Likelihood/Util.h"
#include "Likelihood/StApp.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "LatSc.h"

// #include "flux/SpectrumFactory.h"
// #include "GRB/GRBmanager.h"
// static SpectrumFactory<GRBmanager> my_factory;

ISpectrumFactory & GaussianSourceFactory();
ISpectrumFactory & MapSourceFactory();
ISpectrumFactory & PeriodicSourceFactory();
ISpectrumFactory & PulsarFactory();
ISpectrumFactory & SimpleTransientFactory();
ISpectrumFactory & TransientTemplateFactory();

using Likelihood::Util;

class ObsSim {
public:
   ObsSim(hoops::IParGroup & pars) : m_pars(pars), m_simulator(0) {
      m_count = m_pars["Number_of_events"];
      GaussianSourceFactory();
      MapSourceFactory();
      PeriodicSourceFactory();
      PulsarFactory();
      SimpleTransientFactory();
      TransientTemplateFactory();
   }
   ~ObsSim() {
      delete m_simulator;
      for (unsigned int i = 0; i < m_respPtrs.size(); i++) {
         delete m_respPtrs[i];
      }
   }
   void run();
private:
   hoops::IParGroup & m_pars;
   double m_count;
   std::vector<std::string> m_xmlSourceFiles;
   std::vector<std::string> m_srcNames;
   std::vector<latResponse::Irfs *> m_respPtrs;
   observationSim::Simulator * m_simulator;

   void setRandomSeed();
   void setXmlFiles();
   void readSrcNames();
   void createResponseFuncs();
   void createSimulator();
   void generateData();
};

Likelihood::StApp<ObsSim> my_application("obsSim");

void ObsSim::run() {
   setRandomSeed();
   setXmlFiles();
   readSrcNames();
   createResponseFuncs();
   createSimulator();
   generateData();
   std::cout << "Done." << std::endl;
}

void ObsSim::setRandomSeed() {
// Set the random number seed in the CLHEP random number engine.
// We only do this once per run, so we set it using the constructor.
// See <a href="http://wwwasd.web.cern.ch/wwwasd/lhc++/clhep/doxygen/html/Random_8h-source.html">CLHEP/Random/Random.h</a>.
   HepRandom hepRandom(m_pars["Random_seed"]);
}

void ObsSim::setXmlFiles() {
// observationSim::Simulator requires a specific "TimeCandle" source,
// so time_source.xml must always be loaded.
   m_xmlSourceFiles.push_back("$(OBSERVATIONSIMROOT)/xml/time_source.xml");

// Fetch any user-specified xml file of flux-style source definitions,
// replacing the default list.
   std::string xmlFiles = m_pars["XML_source_file"];
   if (xmlFiles == "none" || xmlFiles == "") { // use the default
      xmlFiles = "$(OBSERVATIONSIMROOT)/xml/xmlFiles.dat";
   }
   facilities::Util::expandEnvVar(&xmlFiles);
   if (Util::fileExists(xmlFiles)) {
      std::vector<std::string> files;
      Util::readLines(xmlFiles, files);
      for (unsigned int i=0; i < files.size(); i++) {
         facilities::Util::expandEnvVar(&files[i]);
         if (Util::fileExists(files[i])) {
            m_xmlSourceFiles.push_back(files[i]);
         } else {
            std::cout << "File not found: " 
                      << files[i] << std::endl;
         }
      }
   } else {
      throw std::invalid_argument("List of XML files not found: " + xmlFiles);
   }
}

void ObsSim::readSrcNames() {
   std::string srcListFile = m_pars["Source_list"];
   if (Util::fileExists(srcListFile)) { 
      Util::readLines(srcListFile, m_srcNames);
      if (m_srcNames.size() == 0) {
         throw std::invalid_argument("No sources given in " + srcListFile);
      }
   } else {
      throw std::invalid_argument("Source_list file " + srcListFile
                                  + " doesn't exist.");
   }
}   

void ObsSim::createResponseFuncs() {
   std::string responseFuncs = m_pars["Response_functions"];
   std::map< std::string, std::vector<std::string> > responseIds;
   responseIds["FRONT"].push_back("DC1::Front");
   responseIds["BACK"].push_back("DC1::Back");
   responseIds["FRONT/BACK"].push_back("DC1::Front");
   responseIds["FRONT/BACK"].push_back("DC1::Back");
   responseIds["NO_EDISP"].push_back("DC1::Front_noEdisp");
   responseIds["NO_EDISP"].push_back("DC1::Back_noEdisp");
   responseIds["GLAST25"].push_back("Glast25::Front");
   responseIds["GLAST25"].push_back("Glast25::Back");
   if (responseIds.count(responseFuncs)) {
      std::vector<std::string> &resps = responseIds[responseFuncs];
      for (unsigned int i = 0; i < resps.size(); i++) {
         m_respPtrs.push_back(latResponse::irfsFactory().create(resps[i]));
      }
   } else {
      throw std::invalid_argument("Invalid response function choice: "
                                  + responseFuncs);
   }
}   

void ObsSim::createSimulator() {
   double totalArea = m_pars["Maximum_effective_area"];
   double startTime = m_pars["Start_time"];
   std::string pointingHistory = m_pars["Pointing_history_file"];
   m_simulator = new observationSim::Simulator(m_srcNames, m_xmlSourceFiles, 
                                               totalArea, startTime, 
                                               pointingHistory);
// // Turn off rocking.
//    my_simulator.setRocking(0);
}

void ObsSim::generateData() {
#ifdef USE_FT1
   bool useFT1(true);
#else
   bool useFT1(false);
#endif
   long nMaxRows = m_pars["Maximum_number_of_rows"];
   std::string prefix = m_pars["Output_file_prefix"];
   observationSim::EventContainer events(prefix + "_events", useFT1, nMaxRows);
   observationSim::ScDataContainer scData(prefix + "_scData", useFT1, 
                                          nMaxRows);
   observationSim::Spacecraft *spacecraft = new observationSim::LatSc();
   if (m_pars["Use_as_sim_time"]) {
      std::cout << "Generating events for a simulation time of "
                << m_count << " seconds...." << std::endl;
      m_simulator->generateEvents(m_count, events, scData, m_respPtrs, 
                                  spacecraft);
   } else {
      std::cout << "Generating " << m_count << " events...." << std::endl;
      m_simulator->generateEvents(static_cast<long>(m_count), events, 
                                  scData, m_respPtrs, spacecraft);
   }
}
