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

#include "hoops/hoops_exception.h"

#include "astro/SkyDir.h"

#include "latResponse/Irfs.h"
#include "latResponse/IrfsFactory.h"

#include "Likelihood/RunParams.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "observationSim/../src/LatSc.h"

int main(int iargc, char * argv[]) {

#ifdef TRAP_FPE
   feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
#endif
   
// Read in the command-line parameters using HOOPS
   std::string filename("obsSim.par");
   delete argv[0];
   argv[0] = strdup(filename.c_str());

   Likelihood::RunParams params(iargc, argv);
         
// Here are the default xml files for flux-style sources.
   std::vector<std::string> xmlSourceFiles;
   xmlSourceFiles.push_back("$(OBSERVATIONSIMROOT)/xml/source_library.xml");
   xmlSourceFiles.push_back("$(OBSERVATIONSIMROOT)/xml/3EG_catalog_30MeV.xml");

// Fetch any user-specified xml file of flux-style source definitions.
   std::string xmlFile;
   params.getParam("XML_source_file", xmlFile);
   if (xmlFile != "" && xmlFile != "none") {
      xmlSourceFiles.push_back(xmlFile);
   }

// Read the file containing the list of sources.
   std::string srcListFile;
   params.getParam("Source_list", srcListFile);
   std::vector<std::string> srcNames;
   Likelihood::RunParams::readLines(srcListFile, srcNames);
   
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
   latResponse::IrfsFactory irfsFactory;
   if (responseFuncs == "COMBINED") {
      respPtrs.push_back(irfsFactory.create("Glast25::Combined"));
   } else if (responseFuncs == "FRONT/BACK") {
      respPtrs.push_back(irfsFactory.create("Glast25::Front"));
      respPtrs.push_back(irfsFactory.create("Glast25::Back"));
   } else if (responseFuncs == "COMBINED_10") {
      respPtrs.push_back(irfsFactory.create("Glast25::Combined_10"));
   } else if (responseFuncs == "FRONT/BACK_10") {
      respPtrs.push_back(irfsFactory.create("Glast25::Front_10"));
      respPtrs.push_back(irfsFactory.create("Glast25::Back_10"));
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
   observationSim::Simulator my_simulator(srcNames, xmlSourceFiles, totalArea,
                                          startTime, pointingHistory);

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
      my_simulator.generateEvents(count, events, scData, respPtrs, spacecraft);
   } else {
      std::cout << "Generating " << count << " events...." << std::endl;
      my_simulator.generateEvents(static_cast<long>(count), events, 
                                  scData, respPtrs, spacecraft);
   }
   std::cout << "Done." << std::endl;
}
