/**
 * @file main.cxx
 * @brief Test program to exercise observationSim interface as a
 * prelude to the O2 tool.
 * @author J. Chiang
 * $Header$
 */

#include "astro/SkyDir.h"

#include "latResponse/../src/Glast25.h"
#include "latResponse/../src/AeffGlast25.h"
#include "latResponse/../src/PsfGlast25.h"
#include "latResponse/../src/EdispGlast25.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "observationSim/../src/LatSc.h"

void help();

int main(int argn, char * argc[]) {
   
// Create list of xml input files for source definitions.
   std::vector<std::string> fileList;
   std::string xml_list("$(OBSERVATIONSIMROOT)/xml/source_library.xml");
   fileList.push_back(xml_list);
   xml_list = "$(OBSERVATIONSIMROOT)/xml/3EG_catalog_32MeV.xml";
   fileList.push_back(xml_list);
   xml_list = "$(OBSERVATIONSIMROOT)/xml/test_sources_v2.xml";
   fileList.push_back(xml_list);
   
// Parse the command line arguments.
//
// The first argument will be the total number of photons or the total
// simulation time in seconds.
//
   long count;
   if (argn > 1) {
      count = static_cast<long>(::atof(argc[1]));
   } else {
      count = 1000;
   }

// All subsequent arguments are either option flags or the names of
// sources.
//
   bool useSimTime(false);
   std::vector<std::string> sourceNames;
   if (argn > 2) {
      for (int i = 2; i < argn; i++) {
         std::string argString = argc[i];
         if (argString == "-t") {  // This is the only option, so far.
            useSimTime = true;
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
   const char *root = ::getenv("LATRESPONSEROOT");
   std::string caldbPath;
   if (!root) {
      caldbPath = "/u1/jchiang/SciTools/dev/latResponse/v0r1/data/CALDB";
   } else {
      caldbPath = std::string(root) + "/data/CALDB";
   }

// Create pointers to the GLAST25 response function objects.  Note
// that there is no GLAST25 energy dispersion file.
   latResponse::IAeff *aeff 
      = new latResponse::AeffGlast25(caldbPath + "/aeff_lat.fits",
                                     latResponse::Glast25::Combined);
   latResponse::IPsf *psf 
      = new latResponse::PsfGlast25(caldbPath + "/psf_lat.fits",
                                    latResponse::Glast25::Combined);
   latResponse::IEdisp *edisp = new latResponse::EdispGlast25();

   latResponse::Irfs response(aeff, psf, edisp);

// Generate the events and spacecraft data.
   observationSim::EventContainer events("test_events", true);
   observationSim::ScDataContainer scData("test_scData", true);

// The spacecraft object.
   observationSim::Spacecraft *spacecraft = new observationSim::LatSc();

// Use simulation time rather than total counts if desired.
   if (useSimTime) {
      std::cout << "Generating events for a simulation time of "
                << count << " seconds....";
      my_simulator.generateEvents(static_cast<double>(count), events, 
                                  scData, response, spacecraft);
   } else {
      std::cout << "Generating " << count << " events....";
      my_simulator.generateEvents(count, events, scData, response, spacecraft);
   }
   std::cout << "Done." << std::endl;
}

void help() {
   std::cerr << "usage: <program name> <counts> [<options> <sourceNames>]"
             << std::endl;
}
