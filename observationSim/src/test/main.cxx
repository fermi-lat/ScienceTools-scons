/**
 * @file main.cxx
 * @brief Test program to exercise observationSim interface as a
 * prelude to the O2 tool.
 * @author J. Chiang
 * $Header$
 */

#include "Likelihood/Response.h"
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
   std::string source_name;
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

// Read in the response data.
   const char *root = ::getenv("LIKELIHOODROOT");
   std::string caldbPath;
   if (!root) {
      caldbPath = "/u1/jchiang/SciToolsDev/Likelihood/v0r8/src/test/CALDB";
   } else {
      caldbPath = std::string(root) + "/src/test/CALDB";
   }

   my_simulator.readResponseData(caldbPath, Likelihood::Response::Combined);

// Generate the events and spacecraft data.
   observationSim::EventContainer events("test_events.dat", true);
   observationSim::ScDataContainer scData("test_scData.dat", true);
   my_simulator.generateEvents(count, events, scData);

}

void help() {
   std::cerr << 
      "   Simple test program to create a particle source, then run it.\n"
      "   Command line args are \n"
      "      <source name> <count>\n" << std::endl;
}
