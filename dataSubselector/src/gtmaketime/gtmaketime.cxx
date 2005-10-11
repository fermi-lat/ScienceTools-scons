/**
 * @file gtmaketime.cxx
 * @brief Create a GTI extension based on a filter expression applied
 * to a spacecraft data file and merge with the existing GTI in the
 * event data file.
 * @author J. Chiang
 *
 * $Header$
 */

#include <memory>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "st_facilities/Util.h"

#include "dataSubselector/Cuts.h"
#include "dataSubselector/Gti.h"
#include "dataSubselector/GtiCut.h"

/**
 * @class MakeTime
 * @author J. Chiang
 *
 */

class MakeTime : public st_app::StApp {
public:
   MakeTime() : st_app::StApp(),
                m_pars(st_app::StApp::getParGroup("gtmaketime")) {
      try {
         m_pars.Prompt();
         m_pars.Save();
         setName("gtmaketime");
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in Maketime constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~MakeTime() throw() {
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
   dataSubselector::Gti m_gti;

   void createGti();
   void mergeGtis();

   static std::string s_cvs_id;
};

std::string MakeTime::s_cvs_id("$Name$");

st_app::StAppFactory<MakeTime> myAppFactory("gtmaketime");

void MakeTime::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void MakeTime::run() {
   createGti();
   mergeGtis();
}

void MakeTime::createGti() {
   std::string scfile = m_pars["scfile"];
   std::vector<std::string> scfiles;
   st_facilities::Util::resolve_fits_files(scfile, scfiles);
   std::string sctable = m_pars["sctable"];
   std::string filter = m_pars["filter"];

   for (unsigned int i = 0; i < scfiles.size(); i++) {
      std::auto_ptr<const tip::Table> 
         in_table(tip::IFileSvc::instance().readTable(scfiles.at(i), 
                                                      sctable, filter));
   
      tip::Table::ConstIterator input = in_table->begin();
      tip::ConstTableRecord & in = *input;

      double start_time;
      double stop_time;
      for (; input != in_table->end(); ++input) {
         in["START"].get(start_time);
         in["STOP"].get(stop_time);
         dataSubselector::Gti gti;
         gti.insertInterval(start_time, stop_time);
         m_gti = m_gti | gti;
      }
   }
}

void MakeTime::mergeGtis() {
   std::string evfile = m_pars["evfile"];
   std::vector<std::string> evfiles;
   st_facilities::Util::resolve_fits_files(evfile, evfiles);
   std::string evtable = m_pars["evtable"];

   for (unsigned int j = 0; j < evfiles.size(); j++) {
      dataSubselector::Cuts cuts(evfiles.at(j), evtable);

      std::vector<const dataSubselector::GtiCut *> gtiCuts;
      cuts.getGtiCuts(gtiCuts);
      
      dataSubselector::Gti my_gti(m_gti);
      for (size_t i = 0; i < gtiCuts.size(); i++) {
         my_gti = my_gti & gtiCuts.at(i)->gti();
      }
      
      my_gti.writeExtension(evfiles.at(j));
   }
}
