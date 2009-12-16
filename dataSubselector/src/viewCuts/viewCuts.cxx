/**
 * @file viewCuts.cxx
 * @brief Simple app to print the DSS keywords in an event extension or
 * other extension.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>
#include <iostream>

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "st_facilities/Util.h"
#include "dataSubselector/Cuts.h"

class ViewCuts : public st_app::StApp {

public:

   ViewCuts() : st_app::StApp(),
                m_pars(st_app::StApp::getParGroup("gtvcut")) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in ViewCuts constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~ViewCuts() throw() {
      try {
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) {
      }
   }

   virtual void run();

   virtual void banner() const;

private:
   
   st_app::AppParGroup & m_pars;

   static std::string s_cvs_id;
};

std::string ViewCuts::s_cvs_id("$Name$");

st_app::StAppFactory<ViewCuts> myAppFactory("gtvcut");

void ViewCuts::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void ViewCuts::run() {
   m_pars.Prompt();
   m_pars.Save();
   std::string infile = m_pars["infile"];
   std::string extname = m_pars["table"];
   bool suppressGtis = m_pars["suppress_gtis"];
   dataSubselector::Cuts cuts(infile, extname, false);
   cuts.writeCuts(std::cout, suppressGtis);
}
