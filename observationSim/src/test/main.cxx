/**
 * @file main.cxx
 * @brief Test program to exercise observationSim interface as a
 * prelude to the O2 tool.
 * @author J. Chiang
 * $Header$
 */
#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <cstdlib>

#include "astro/SkyDir.h"

#include "latResponse/Irfs.h"
#include "latResponse/IrfsFactory.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "LatSc.h"

using latResponse::irfsFactory;

void help();

int main(int iargc, char * argv[]) {
#ifdef TRAP_FPE
   feenableexcept (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW);
#endif
   
// Create list of xml input files for source definitions.
   std::vector<std::string> fileList;
   std::string xml_list("$(OBSERVATIONSIMROOT)/xml/obsSim_source_library.xml");
   fileList.push_back(xml_list);
   xml_list = "$(OBSERVATIONSIMROOT)/xml/3EG_catalog_32MeV.xml";
   fileList.push_back(xml_list);
   xml_list = "$(OBSERVATIONSIMROOT)/xml/test_sources.xml";
   fileList.push_back(xml_list);
   xml_list = "$(OBSERVATIONSIMROOT)/xml/time_source.xml";
   fileList.push_back(xml_list);
   
// Parse the command line arguments.
//
// The first argument will be the total number of photons or the total
// simulation time in seconds.
//
   long count;
   if (iargc > 1) {
      count = static_cast<long>(std::atof(argv[1]));
   } else {
      count = 1000;
   }

// All subsequent arguments are either option flags or the names of
// sources.
//
   bool useSimTime(false);
   bool useCombined(true);
   std::vector<std::string> sourceNames;
   if (iargc > 2) {
      for (int i = 2; i < iargc; i++) {
         std::string argString = argv[i];
         if (argString == "-t") {
// Interpret arg[1] as elapsed time in seconds.
            useSimTime = true;
         } else if (argString == "-fb") {
// Use Front/Back responses instead of Combined.
            useCombined = false;
         } else {
// Assume the next argument is a source name or a request for help.
            if (argString == "help") {
               help();
               return 0;
            }
            sourceNames.push_back(argString);
         }
      }
   } else {
      sourceNames.push_back("all_3EG_sources");
//      sourceNames.push_back("galdiffusemap");
   }

// Create the Simulator object
   observationSim::Simulator my_simulator(sourceNames, fileList);

// Ascertain paths to GLAST25 response files.
   const char *root = std::getenv("LATRESPONSEROOT");
   std::string caldbPath;
   if (!root) {
      caldbPath = "/u1/jchiang/SciTools/dev/latResponse/v0r1/data/CALDB";
   } else {
      caldbPath = std::string(root) + "/data/CALDB";
   }

// Allow for multiple IRFs.
   std::vector<latResponse::Irfs *> respPtrs;
   if (useCombined) {
      respPtrs.push_back(irfsFactory().create("Glast25::Combined"));
   } else { // use Front & Back
      respPtrs.push_back(irfsFactory().create("Glast25::Front"));
      respPtrs.push_back(irfsFactory().create("Glast25::Back"));
   }

// Generate the events and spacecraft data.
#ifdef USE_FT1
   bool useFT1(true);
#else
   bool useFT1(false);
#endif
   observationSim::EventContainer events("test_events", useFT1);
   observationSim::ScDataContainer scData("test_scData", useFT1);

// The spacecraft object.
   observationSim::Spacecraft *spacecraft = new observationSim::LatSc();

// Use simulation time rather than total counts if desired.
   if (useSimTime) {
      std::cout << "Generating events for a simulation time of "
                << count << " seconds....";
      my_simulator.generateEvents(static_cast<double>(count), events, 
                                  scData, respPtrs, spacecraft);
   } else {
      std::cout << "Generating " << count << " events....";
      my_simulator.generateEvents(count, events, scData, respPtrs, spacecraft);
   }
   std::cout << "Done." << std::endl;
}

void help() {
   std::cerr << "usage: <program name> <counts> [<options> <sourceNames>]\n"
             << "options: \n"
             << "  -t interpret counts as elapsed time in seconds\n" 
             << "  -fb use Front/Back IRFs\n"
             << std::endl;
}
