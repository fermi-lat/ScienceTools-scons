/**
 * @file orbSim.cxx
 * @brief A prototype O2 application.
 * @author J. Chiang
 *
 * $Header$
 */

#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <sstream>
#include <stdexcept>
#include <utility>

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_facilities/Util.h"

#include "astro/GPS.h"
#include "astro/SkyDir.h"

#include "irfInterface/IrfsFactory.h"

#include "observationSim/Simulator.h"
#include "observationSim/EventContainer.h"
#include "observationSim/ScDataContainer.h"
#include "LatSc.h"

class OrbSim : public st_app::StApp {
public:
   OrbSim() : st_app::StApp(), m_pars(st_app::StApp::getParGroup("gtorbsim")),
              m_simulator(0),
              m_formatter(new st_stream::StreamFormatter("gtorbsim", "", 2)) {
      setVersion(s_cvs_id);
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
   virtual void banner() const;
private:
   st_app::AppParGroup & m_pars;
   double m_count;
   observationSim::Simulator * m_simulator;
   st_stream::StreamFormatter * m_formatter;

   std::map<std::string, int> m_rockTypes;
   std::vector<irfInterface::Irfs *> m_respPtrs;

   void defineRockTypes();
   void promptForParameters();
   void checkOutputFiles();
   void createSimulator();
   void generateData();

   static std::string s_cvs_id;
};

st_app::StAppFactory<OrbSim> myAppFactory("gtorbsim");

std::string OrbSim::s_cvs_id("$Name$");

void OrbSim::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void OrbSim::run() {
   defineRockTypes();
   promptForParameters();
   checkOutputFiles();
   createSimulator();
   generateData();
   m_formatter->info() << "Done." << std::endl;
}

void OrbSim::promptForParameters() {
   m_pars.Prompt("outfile_prefix");
   m_pars.Prompt("pointing_strategy");
   std::string pointing_strategy = m_pars["pointing_strategy"];
   if (pointing_strategy != "POINT") {
      m_pars.Prompt("rocking_angle");
   } else {
      m_pars.Prompt("ra");
      m_pars.Prompt("dec");
   }
   m_pars.Prompt("simulation_time");
   m_pars.Save();
   m_count = m_pars["simulation_time"];
}

void OrbSim::checkOutputFiles() {
   bool clobber = m_pars["clobber"];
   if (!clobber) {
      std::string prefix = m_pars["outfile_prefix"];
      std::string file = prefix + "_scData_0000.fits";
      if (st_facilities::Util::fileExists(file)) {
         m_formatter->err() << "Output file " << file << " already exists,\n"
                            << "and you have set 'clobber' to 'no'.\n"
                            << "Please provide a different output file prefix."
                            << std::endl;
         std::exit(1);
      }
   }
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
   double startTime = m_pars["start_time"];
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
   std::string pointing_strategy = m_pars["pointing_strategy"];
   int rockType = m_rockTypes[pointing_strategy];
   double rockingAngle = m_pars["rocking_angle"];
   m_simulator->setRocking(rockType, rockingAngle);
   if (pointing_strategy == "POINT") {
      double ra = m_pars["ra"];
      double dec = m_pars["dec"];
      astro::SkyDir dir(ra, dec);
// // GPS wants (l, b) for the rotation angles in pointing mode, but
// // they must be in radians!
//       astro::GPS::instance()->rotateAngles(std::make_pair(dir.l()*M_PI/180., 
//                                                           dir.b()*M_PI/180.));

      throw std::runtime_error("If you are trying to use the 'POINT' pointing"
                               "strategy (so-called),\n it is no longer "
                               "available. If you are curious why, then "
                               "ask Toby Burnett, "
                               "tburnett@u.washington.edu, why he has diabled\n"
                               "the GPS::rotateAngles member function "
                               "as of Oct 5, 2006");
   }
}

void OrbSim::generateData() {
   long nMaxRows = m_pars["max_numrows"];
   std::string prefix = m_pars["outfile_prefix"];
   std::string ev_table("EVENTS");
   std::string sc_table = m_pars["sctable"];
   observationSim::EventContainer events(prefix + "_events", ev_table,
                                         0, nMaxRows);
   events.setAppName("gtorbsim");
   events.setVersion(getVersion());
   observationSim::ScDataContainer scData(prefix + "_scData", sc_table, 
                                          nMaxRows);
   scData.setAppName("gtorbsim");
   scData.setVersion(getVersion());
   observationSim::Spacecraft * spacecraft = new observationSim::LatSc();
   double frac = m_pars["livetime_frac"];
   spacecraft->setLivetimeFrac(frac);
   m_formatter->info() << "Generating pointing history for a "
                       << "simulation time of "
                       << m_count << " seconds...." 
                       << std::endl;
   m_simulator->generateEvents(m_count, events, scData, m_respPtrs, 
                               spacecraft);

// Pad with one more row of ScData.
   double time = scData.simTime() + 30.;
   scData.addScData(time, spacecraft);
   delete spacecraft;
}
