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

void help();

int main(int argn, char * argc[]) {
   
// Create list of xml input files for source definitions.
   std::vector<std::string> fileList;
   std::string xml_list("$(OBSERVATIONSIMROOT)/xml/source_library.xml");
   fileList.push_back(xml_list);
   xml_list = "$(OBSERVATIONSIMROOT)/xml/3EG_catalog_32MeV.xml";
   fileList.push_back(xml_list);
   
// Parse the command line arguments.
// Obtain the source name (or request for help).
   std::string source_name = "all_3EG_sources";
   if (argn > 1) source_name = argc[1];
   if (source_name == "help") { 
      help();
      return 0;
   }

// Create the Simulator object
   observationSim::Simulator my_simulator(source_name, fileList);

// Get the number of photons to accumulate or use the default value of 10.
   long count;
   if (argn > 2) {
      count = static_cast<long>(::atof(argc[2]));
   } else {
      count = 10;
   }

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
   observationSim::EventContainer events("test_events.dat", true);
   observationSim::ScDataContainer scData("test_scData.dat", true);

// Use simulation time rather than total counts if desired.
   if (argn == 4 && std::string(argc[3]) == "-t") {
      my_simulator.generateEvents(static_cast<double>(count), events, 
                                  scData, response);
   } else {
      my_simulator.generateEvents(count, events, scData, response);
   }
}

void help() {
   std::cerr << 
      "   Simple test program to create a particle source, then run it.\n"
      "   Command line args are \n"
      "      <source name> <count>\n" << std::endl;
}
