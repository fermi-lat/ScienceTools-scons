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
#include <stdexcept>

#include "CLHEP/Random/Random.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "facilities/Util.h"

#include "astro/SkyDir.h"

#include "irfInterface/IrfsFactory.h"
#include "irfLoader/Loader.h"

#include "st_facilities/Util.h"

#include "dataSubselector/Cuts.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"

#include "LatSc.h"
#include "Verbosity.h"

ISpectrumFactory & GaussianSourceFactory();
ISpectrumFactory & GRBmanagerFactory();
ISpectrumFactory & IsotropicFactory();
ISpectrumFactory & MapSourceFactory();
ISpectrumFactory & PeriodicSourceFactory();
ISpectrumFactory & PulsarFactory();
ISpectrumFactory & SimpleTransientFactory();
ISpectrumFactory & TransientTemplateFactory();
ISpectrumFactory & PulsarSpectrumFactory();

using st_facilities::Util;

class ObsSim : public st_app::StApp {
public:
   ObsSim() : st_app::StApp(), m_pars(st_app::StApp::getParGroup("obsSim")),
              m_simulator(0) {
   }
   virtual ~ObsSim() throw() {
      try {
         delete m_simulator;
         for (unsigned int i = 0; i < m_respPtrs.size(); i++) {
            delete m_respPtrs[i];
         }
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) { 
      }
   }
   virtual void run();
private:
   st_app::AppParGroup & m_pars;
   double m_count;
   std::vector<std::string> m_xmlSourceFiles;
   std::vector<std::string> m_srcNames;
   std::vector<irfInterface::Irfs *> m_respPtrs;
   observationSim::Simulator * m_simulator;

   void promptForParameters();
   void checkOutputFiles();
   void setRandomSeed();
   void createFactories();
   void setXmlFiles();
   void readSrcNames();
   void createResponseFuncs();
   void createSimulator();
   void generateData();
};

st_app::StAppFactory<ObsSim> myAppFactory;

void ObsSim::run() {
   promptForParameters();
   checkOutputFiles();
   observationSim::Verbosity::instance(m_pars["chatter"]);
   setRandomSeed();
   createFactories();
   setXmlFiles();
   readSrcNames();
   createResponseFuncs();
   createSimulator();
   generateData();
   if (observationSim::print_output()) {
      std::cout << "Done." << std::endl;
   }
}

void ObsSim::promptForParameters() {
   m_pars.Prompt("xml_source_file");
   m_pars.Prompt("source_list");
   m_pars.Prompt("scfile");
   m_pars.Prompt("outfile_prefix");
   m_pars.Prompt("simulation_time");
   m_pars.Prompt("use_acceptance_cone");
   if (m_pars["use_acceptance_cone"]) {
      m_pars.Prompt("ra");
      m_pars.Prompt("dec");
      m_pars.Prompt("radius");
   }
   m_pars.Prompt("rspfunc");
   m_pars.Prompt("random_seed");
   m_pars.Save();
   m_count = m_pars["simulation_time"];
}

void ObsSim::checkOutputFiles() {
   bool clobber = m_pars["clobber"];
   if (!clobber) {
      std::string prefix = m_pars["outfile_prefix"];
      std::string file = prefix + "_events_0000.fits";
      if (st_facilities::Util::fileExists(file)) {
         std::cout << "Output file " << file 
                   << " already exists and you have set 'clobber' to 'no'.\n"
                   << "Please provide a different output file prefix." 
                   << std::endl;
         std::exit(1);
      }
      file = prefix + "_scData_0000.fits";
      if (st_facilities::Util::fileExists(file)) {
         std::cout << "Output file " << file 
                   << " already exists and you have set 'clobber' to 'no'.\n"
                   << "Please provide a different output file prefix." 
                   << std::endl;
         std::exit(1);
      }
   }
}


void ObsSim::setRandomSeed() {
// Set the random number seed in the CLHEP random number engine.
// We only do this once per run, so we set it using the constructor.
// See <a href="http://wwwasd.web.cern.ch/wwwasd/lhc++/clhep/doxygen/html/Random_8h-source.html">CLHEP/Random/Random.h</a>.
   HepRandom hepRandom(m_pars["random_seed"]);
}

void ObsSim::createFactories() {
   GaussianSourceFactory();
   GRBmanagerFactory();
   IsotropicFactory();
   MapSourceFactory();
   PeriodicSourceFactory();
   PulsarFactory();
   SimpleTransientFactory();
   TransientTemplateFactory();
   PulsarSpectrumFactory();
}

void ObsSim::setXmlFiles() {
// observationSim::Simulator requires a specific "TimeCandle" source,
// so time_source.xml must always be loaded.
   m_xmlSourceFiles.push_back("$(OBSERVATIONSIMROOT)/xml/time_source.xml");

// Fetch any user-specified xml file of flux-style source definitions,
// replacing the default list.
   std::string xmlFiles = m_pars["xml_source_file"];
   if (xmlFiles == "none" || xmlFiles == "") { // use the default
      xmlFiles = "$(OBSERVATIONSIMROOT)/xml/xmlFiles.dat";
   }
   facilities::Util::expandEnvVar(&xmlFiles);
   if (Util::fileExists(xmlFiles)) {
      if (Util::isXmlFile(xmlFiles)) {
         m_xmlSourceFiles.push_back(xmlFiles);
      } else {
         std::vector<std::string> files;
         Util::readLines(xmlFiles, files);
         for (unsigned int i=0; i < files.size(); i++) {
            facilities::Util::expandEnvVar(&files[i]);
            if (Util::fileExists(files[i])) {
               m_xmlSourceFiles.push_back(files[i]);
            } else {
               if (observationSim::print_output()) {
                  std::cout << "File not found: " 
                            << files[i] << std::endl;
               }
            }
         }
      } 
   } else {
      throw std::invalid_argument("List of XML files not found: " + xmlFiles);
   }
}

void ObsSim::readSrcNames() {
   std::string srcListFile = m_pars["source_list"];
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
   irfLoader::Loader::go();
   irfInterface::IrfsFactory * myFactory 
      = irfInterface::IrfsFactory::instance();

   std::string responseFuncs = m_pars["rspfunc"];

   typedef std::map< std::string, std::vector<std::string> > respMap;
   const respMap & responseIds = irfLoader::Loader::respIds();
   respMap::const_iterator it;
   if ( (it = responseIds.find(responseFuncs)) != responseIds.end() ) {
      const std::vector<std::string> & resps = it->second;
      for (unsigned int i = 0; i < resps.size(); i++) {
         m_respPtrs.push_back(myFactory->create(resps[i]));
      }
   } else {
      throw std::invalid_argument("Invalid response function choice: "
                                  + responseFuncs);
   }
}   

void ObsSim::createSimulator() {
   double totalArea = m_pars["max_effarea"];
   double startTime = m_pars["start_time"];
   std::string pointingHistory = m_pars["scfile"];
   double maxSimTime = 3.155e8;
   try {
      maxSimTime = m_pars["max_simulation_time"];
   } catch (std::exception & eObj) {
   }
   m_simulator = new observationSim::Simulator(m_srcNames, m_xmlSourceFiles, 
                                               totalArea, startTime, 
                                               pointingHistory, maxSimTime);
}

void ObsSim::generateData() {
   long nMaxRows = m_pars["max_numrows"];
   std::string prefix = m_pars["outfile_prefix"];
   dataSubselector::Cuts * cuts = new dataSubselector::Cuts;
   cuts->addRangeCut("ENERGY", "MeV", m_pars["emin"], m_pars["emax"]);
   if (m_pars["use_acceptance_cone"]) {
      cuts->addSkyConeCut(m_pars["ra"], m_pars["dec"], m_pars["radius"]);
   }
   double start_time(m_pars["start_time"]);
   double stop_time;
   if (m_pars["use_as_numevents"]) {
      stop_time = start_time;
   } else {
      double sim_time(m_pars["simulation_time"]);  // yes, this is BS.
      stop_time = start_time + sim_time;
   }
   observationSim::EventContainer events(prefix + "_events", cuts, nMaxRows,
                                         start_time, stop_time);
   std::string pointingHistory = m_pars["scfile"];
   bool writeScData = (pointingHistory == "" || pointingHistory == "none");
   observationSim::ScDataContainer scData(prefix + "_scData", nMaxRows,
                                          writeScData);
   observationSim::Spacecraft * spacecraft = new observationSim::LatSc();
   if (m_pars["use_as_numevents"]) {
      if (observationSim::print_output()) {
         std::cout << "Generating " << m_count << " events...." << std::endl;
      }
      m_simulator->generateEvents(static_cast<long>(m_count), events, 
                                  scData, m_respPtrs, spacecraft);
   } else {
      if (observationSim::print_output()) {
         std::cout << "Generating events for a simulation time of "
                   << m_count << " seconds...." << std::endl;
      }
      m_simulator->generateEvents(m_count, events, scData, m_respPtrs, 
                                  spacecraft);
   }

// Pad with one more row of ScData.
   double time = scData.simTime() + 30.;
   scData.addScData(time, spacecraft);
}
