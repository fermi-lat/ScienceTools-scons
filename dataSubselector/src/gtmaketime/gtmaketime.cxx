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

#include "st_stream/StreamFormatter.h"

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
   std::string m_evfile;
   std::string m_outfile;

   void check_outfile();
   void createGti();
   void mergeGtis();
   void copyTable() const;

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
   m_pars.Prompt();
   m_pars.Save();
   check_outfile();
   createGti();
   mergeGtis();
   copyTable();
}

void MakeTime::check_outfile() {
   st_stream::StreamFormatter formatter("MakeTime", "run", 2);
   bool clobber = m_pars["clobber"];
   std::string outfile = m_pars["outfile"];
   m_outfile = outfile;
   if (outfile == "") {
      formatter.err() << "Please specify an output file name." 
                      << std::endl;
      std::exit(1);
   }
   if (!clobber && st_facilities::Util::fileExists(outfile)) {
      formatter.err() << "Output file " << outfile 
                      << " exists and clobber is set to 'no'.  Exiting."
                      << std::endl;
      std::exit(1);
   }
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
   m_evfile = evfile;
   std::string evtable = m_pars["evtable"];

   dataSubselector::Cuts cuts(evfile, evtable);
   
   std::vector<const dataSubselector::GtiCut *> gtiCuts;
   cuts.getGtiCuts(gtiCuts);
   
   for (size_t i = 0; i < gtiCuts.size(); i++) {
      m_gti = m_gti & gtiCuts.at(i)->gti();
   }
}

void MakeTime::copyTable() const {
   tip::IFileSvc::instance().createFile(m_outfile, m_evfile);

   std::string extension("EVENTS");
   const tip::Table * inputTable 
      = tip::IFileSvc::instance().readTable(m_evfile, extension);
   
   tip::Table * outputTable 
      = tip::IFileSvc::instance().editTable(m_outfile, extension);

   outputTable->setNumRecords(inputTable->getNumRecords());

   tip::Table::ConstIterator inputIt = inputTable->begin();
   tip::Table::Iterator outputIt = outputTable->begin();

   tip::ConstTableRecord & input = *inputIt;
   tip::Table::Record & output = *outputIt;

   long npts(0);

   dataSubselector::Cuts my_cuts;
   my_cuts.addGtiCut(m_gti);

   for (; inputIt != inputTable->end(); ++inputIt) {
      if (my_cuts.accept(input)) {
         output = input;
         ++outputIt;
         npts++;
      }
   }
// Resize output table to account for filtered rows.
   outputTable->setNumRecords(npts);

   delete inputTable;
   delete outputTable;

   m_gti.writeExtension(m_outfile);
}
