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

// Fetch the filenames for the xml input files containing the
// flux-style source definitions.
   std::string xmlInputFiles = params.string_par("XML_source_filenames");
   std::vector<std::string> fileList;
   Likelihood::RunParams::readLines(xmlInputFiles, fileList);

// Read the file containing the list of sources.
   std::string srcListFile = params.string_par("Source_list");
   std::vector<std::string> srcNames;
   Likelihood::RunParams::readLines(srcListFile, srcNames);
   
// Get the number of events.
   long count = static_cast<long>(params.double_par("Number_of_events"));

// Get the flag to interpret nevents as simulation time in seconds.
   bool useSimTime = params.bool_par("Use_as_sim_time");

// Ascertain which response functions to use.
   std::string responseFuncs = params.string_par("Response_functions");
   std::vector<latResponse::Irfs *> respPtrs;
   latResponse::IrfsFactory irfsFactory;
   if (responseFuncs == "COMBINED") {
      respPtrs.push_back(irfsFactory.create("Glast25::Combined"));
   } else if (responseFuncs == "FRONT/BACK") {
      respPtrs.push_back(irfsFactory.create("Glast25::Front"));
      respPtrs.push_back(irfsFactory.create("Glast25::Back"));
   } else {
      std::cerr << "Invalid response function choice: "
                << responseFuncs << std::endl;
      assert(false);
   }

// Create the Simulator object
   double totalArea = params.double_par("Maximum_effective_area");
   double startTime = params.double_par("Start_time");
   std::string pointingHistory = params.string_par("Pointing_history_file");
   observationSim::Simulator my_simulator(srcNames, fileList, totalArea,
                                          startTime, pointingHistory);

// Generate the events and spacecraft data.
   bool useGoodi = false;
   long nMaxRows = params.long_par("Maximum_number_of_rows");
   std::string prefix = params.string_par("Output_file_prefix");
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
      my_simulator.generateEvents(static_cast<double>(count), events, 
                                  scData, respPtrs, spacecraft);
   } else {
      std::cout << "Generating " << count << " events...." << std::endl;
      my_simulator.generateEvents(count, events, scData, respPtrs, spacecraft);
   }
   std::cout << "Done." << std::endl;
}
