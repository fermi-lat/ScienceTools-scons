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

#include "hoops/hoops_exception.h"

#include "hoopsUtil/RunParams.h"

#include "astro/SkyDir.h"

#include "latResponse/Irfs.h"
#include "latResponse/IrfsFactory.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "observationSim/../src/LatSc.h"

namespace {
   void readLines(std::string inputFile, 
                  std::vector<std::string> &lines) {

      facilities::Util::expandEnvVar(&inputFile);

      std::ifstream file(inputFile.c_str());
      lines.clear();
      std::string line;
      while (std::getline(file, line, '\n')) {
         if (line != "" && line != " ") { //skip (most) blank lines
            lines.push_back(line);
         }
      }
   }

   bool fileExists(const std::string &filename) {
      std::ifstream file(filename.c_str());
      return file.is_open();
   }
} // unnamed namespace

int main(int iargc, char * argv[]) {

#ifdef TRAP_FPE
   feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
#endif
   
// Read in the command-line parameters using HOOPS
   strcpy(argv[0], "obsSim");
   try {
      hoopsUtil::RunParams params(iargc, argv);

// Set the random number seed in the CLHEP random number engine.
// We only do this once per run, so we set it using the constructor.
// See <a href="http://wwwasd.web.cern.ch/wwwasd/lhc++/clhep/doxygen/html/Random_8h-source.html">CLHEP/Random/Random.h</a>.
      long seed;
      params.getParam("Random_seed", seed);
      HepRandom hepRandom(seed);

// observationSim::Simulator requires a specific "TimeCandle" source,
// so time_source.xml must always be loaded.
      std::vector<std::string> xmlSourceFiles;
      xmlSourceFiles.push_back("$(OBSERVATIONSIMROOT)/xml/time_source.xml");

// Fetch any user-specified xml file of flux-style source definitions,
// replacing the default list.
      std::string xmlFiles;
      params.getParam("XML_source_file", xmlFiles);
      if (xmlFiles == "none" || xmlFiles == "") { // use the default
         xmlFiles = "$(OBSERVATIONSIMROOT)/xml/xmlFiles.dat";
      }
      facilities::Util::expandEnvVar(&xmlFiles);
      if (::fileExists(xmlFiles)) {
         std::vector<std::string> files;
         ::readLines(xmlFiles, files);
         for (unsigned int i=0; i < files.size(); i++) {
            facilities::Util::expandEnvVar(&files[i]);
            if (::fileExists(files[i])) {
               xmlSourceFiles.push_back(files[i]);
            } else {
               std::cout << "File not found: " 
                         << files[i] << std::endl;
            }
         }
      } else {
         std::cout << "List of XML files not found: " 
                   <<  xmlFiles << std::endl;
         exit(-1);
      }
// Read the file containing the list of sources.
      std::string srcListFile;
      params.getParam("Source_list", srcListFile);
      std::vector<std::string> srcNames;
      if (::fileExists(srcListFile)) { 
         ::readLines(srcListFile, srcNames);
         if (srcNames.size() == 0) {
            std::cout << "No sources given in " << srcListFile;
            assert(srcNames.size() != 0);
         }
      } else {
         std::cout << "Source_list file " << srcListFile
                   << " doesn't exist." << std::endl;
         assert(::fileExists(srcListFile));
      }
   
// Get the number of events.
      double count;
      params.getParam("Number_of_events", count);

// Get the flag to interpret nevents as simulation time in seconds.
      bool useSimTime;
      params.getParam("Use_as_sim_time", useSimTime);

// Ascertain which response functions to use.
      std::string responseFuncs;
      params.getParam("Response_functions", responseFuncs);
      std::vector<latResponse::Irfs *> respPtrs;

      std::map< std::string, std::vector<std::string> > responseIds;
      responseIds["FRONT"].push_back("DC1::Front");
      responseIds["BACK"].push_back("DC1::Back");
      responseIds["FRONT/BACK"].push_back("DC1::Front");
      responseIds["FRONT/BACK"].push_back("DC1::Back");
      responseIds["NO_EDISP"].push_back("DC1::Front_noEdisp");
      responseIds["NO_EDISP"].push_back("DC1::Back_noEdisp");

      if (responseIds.count(responseFuncs)) {
         std::vector<std::string> &resps = responseIds[responseFuncs];
         for (unsigned int i = 0; i < resps.size(); i++) {
            respPtrs.push_back(latResponse::irfsFactory().create(resps[i]));
         }
      } else {
         std::cerr << "Invalid response function choice: "
                   << responseFuncs << std::endl;
         assert(false);
      }

// Create the Simulator object
      double totalArea;
      params.getParam("Maximum_effective_area", totalArea);
      double startTime;
      params.getParam("Start_time", startTime);
      std::string pointingHistory;
      params.getParam("Pointing_history_file", pointingHistory);
      observationSim::Simulator my_simulator(srcNames, xmlSourceFiles, 
                                             totalArea, startTime, 
                                             pointingHistory);
// Turn off rocking.
//      my_simulator.setRocking(0);
      
// Generate the events and spacecraft data.
#ifdef USE_GOODI
      bool useGoodi(true);
#else
      bool useGoodi(false);
#endif
      long nMaxRows;
      params.getParam("Maximum_number_of_rows", nMaxRows);
      std::string prefix;
      params.getParam("Output_file_prefix", prefix);
      observationSim::EventContainer events(prefix + "_events", 
                                            useGoodi, nMaxRows);
      observationSim::ScDataContainer scData(prefix + "_scData", 
                                             useGoodi, nMaxRows);
// The spacecraft object.
      observationSim::Spacecraft *spacecraft = new observationSim::LatSc();
      
// Use simulation time rather than total counts if desired.
      if (useSimTime) {
         std::cout << "Generating events for a simulation time of "
                   << count << " seconds...." << std::endl;
         my_simulator.generateEvents(count, events, scData, respPtrs, 
                                     spacecraft);
      } else {
         std::cout << "Generating " << count << " events...." << std::endl;
         my_simulator.generateEvents(static_cast<long>(count), events, 
                                     scData, respPtrs, spacecraft);
      }
      std::cout << "Done." << std::endl;
   } catch (hoops::Hexception &eObj) {
      std::cout << "hoops::Hexception: "
                << eObj.Msg() << std::endl;
      std::cout << "hoops::Hexception "
                << "code: " << eObj.Code() << std::endl;
      assert(eObj.Code() != 0);
   }
}
