/**
 * @file orbSim.cxx
 * @brief A prototype O1 application.
 * @author J. Chiang
 *
 * $Header$
 */

#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <utility>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "astro/GPS.h"
#include "astro/SkyDir.h"

#include "irfInterface/IrfsFactory.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "LatSc.h"

class OrbSim : public st_app::StApp {
public:
   OrbSim() : st_app::StApp(), m_pars(st_app::StApp::getParGroup("orbSim")),
              m_simulator(0) {
      m_pars.Prompt();
      m_pars.Save();
      m_count = m_pars["simulation_time"];
   }
   virtual ~OrbSim() throw() {
      try {
         delete m_simulator;
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) { 
      }
   }
   virtual void run();
private:
   st_app::AppParGroup & m_pars;
   double m_count;
   observationSim::Simulator * m_simulator;
   std::map<std::string, int> m_rockTypes;
   std::vector<irfInterface::Irfs *> m_respPtrs;

   void defineRockTypes();
   void createSimulator();
   void generateData();
};

st_app::StAppFactory<OrbSim> myAppFactory;

void OrbSim::run() {
   defineRockTypes();
   createSimulator();
   generateData();
   std::cout << "Done." << std::endl;
}

void OrbSim::defineRockTypes() {
   m_rockTypes["NONE"] = 0;
   m_rockTypes["UPDOWN"] = 1;
   m_rockTypes["SLEWING"] = 2;
   m_rockTypes["ONEPERORBIT"] = 3;
   m_rockTypes["EXPLICIT"] = 4;
   m_rockTypes["POINT"] = 5;
}

void OrbSim::createSimulator() {
   double startTime = m_pars["Start_time"];
   std::string pointingHistory = "none";
   std::vector<std::string> srcNames;
   srcNames.push_back("null_source");
   std::vector<std::string> xmlSourceFiles;
   xmlSourceFiles.push_back("$(OBSERVATIONSIMROOT)/xml/time_source.xml");
   double totalArea(1.21);
   m_simulator = new observationSim::Simulator(srcNames, xmlSourceFiles, 
                                               totalArea, startTime, 
                                               pointingHistory);
// Set pointing strategy.
   std::string pointing_strategy = m_pars["Pointing_strategy"];
   int rockType = m_rockTypes[pointing_strategy];
   double rockingAngle = m_pars["Rocking_angle"];
   m_simulator->setRocking(rockType, rockingAngle);
   if (pointing_strategy == "POINT") {
      double ra = m_pars["ra"];
      double dec = m_pars["dec"];
      astro::SkyDir dir(ra, dec);
      GPS::instance()->rotateAngles(std::make_pair(dir.l(), dir.b()));
   }
}

void OrbSim::generateData() {
   long nMaxRows = m_pars["Maximum_number_of_rows"];
   std::string prefix = m_pars["Output_file_prefix"];
   observationSim::EventContainer events(prefix + "_events", nMaxRows);
   observationSim::ScDataContainer scData(prefix + "_scData", nMaxRows);
   observationSim::Spacecraft * spacecraft = new observationSim::LatSc();
   std::cout << "Generating pointing history for a simulation time of "
             << m_count << " seconds...." << std::endl;
   m_simulator->generateEvents(m_count, events, scData, m_respPtrs, 
                               spacecraft);

// Pad with one more row of ScData.
   double time = scData.simTime() + 30.;
   scData.addScData(time, spacecraft);
   delete spacecraft;
}
