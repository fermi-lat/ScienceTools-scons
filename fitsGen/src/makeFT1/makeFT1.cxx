/**
 * @file makeFT1.cxx
 * @brief Convert merit ntuple to FT1 format.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cctype>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "facilities/Util.h"

#include "dataSubselector/Gti.h"
#include "dataSubselector/Cuts.h"

#include "astro/SkyDir.h"

#include "st_facilities/Env.h"
#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "st_stream/StreamFormatter.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "fitsGen/Ft1File.h"
#include "fitsGen/MeritFile.h"
#include "fitsGen/EventClassifier.h"

using namespace fitsGen;

namespace {
   void toLower(std::string & name) {
      for (std::string::iterator it = name.begin(); it != name.end(); ++it) {
         *it = std::tolower(*it);
      }
   }
   void getFT1Dict(const std::string & inputFile,
                   std::map<std::string, std::string> & ft1Dict) {
      ft1Dict.clear();
      std::vector<std::string> lines;
      st_facilities::Util::readLines(inputFile, lines, "#", true);
      for (size_t i = 0; i < lines.size(); i++) {
         std::vector<std::string> tokens;
         facilities::Util::stringTokenize(lines.at(i), " \t", tokens);
         ft1Dict[tokens.at(0)] = tokens.at(1);
      }
   }
   std::string filterString(const std::string & filterFile) {
      std::ostringstream filter;
      std::vector<std::string> lines;
      st_facilities::Util::readLines(filterFile, lines, "#", true);
      for (size_t i = 0; i < lines.size(); i++) {
         filter << lines.at(i);
      }
      return filter.str();
   }
   void addNeededFields(Ft1File & ft1,
                        const std::map<std::string, std::string> & ft1Dict) {
      const std::vector<std::string> & validFields(ft1.getFieldNames());
      std::map<std::string, std::string>::const_iterator ft1_entry;
      for (ft1_entry = ft1Dict.begin(); ft1_entry != ft1Dict.end();
           ++ft1_entry) {
         std::string candidate(ft1_entry->first);
         toLower(candidate);
         if (std::find(validFields.begin(), validFields.end(), 
                       candidate) == validFields.end()) {
            ft1.appendField(ft1_entry->first, "E");
         }
      }
   }
}

class MakeFt1 : public st_app::StApp {
public:
   MakeFt1() : st_app::StApp(),
               m_pars(st_app::StApp::getParGroup("makeFT1")) {
      try {
         setVersion(s_cvs_id);
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in MakeFT1 constructor." 
                   << std::endl;
         std::exit(1);
      }
   }
   virtual ~MakeFt1() throw() {
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

std::string MakeFt1::s_cvs_id("$Name$");

st_app::StAppFactory<MakeFt1> myAppFactory("makeFT1");

void MakeFt1::banner() const {
   int verbosity = m_pars["chatter"];
   if (verbosity > 2) {
      st_app::StApp::banner();
   }
}

void MakeFt1::run() {
   m_pars.Prompt();
   m_pars.Save();
   std::string rootFile = m_pars["rootFile"];
   std::string fitsFile = m_pars["fitsFile"];
   std::string eventClassifier = m_pars["event_classifier"];
   std::string defaultFilter = m_pars["TCuts"];

   std::string dataDir(st_facilities::Env::getDataDir("fitsGen"));

   if (defaultFilter == "DEFAULT") {
      defaultFilter = st_facilities::Env::appendFileName(dataDir,
                                                         "pass4_cuts");
   }
   std::string filter;
   if (!st_facilities::Util::fileExists(defaultFilter)) {
      filter = defaultFilter;
   } else {
      filter = filterString(defaultFilter);
   }

   st_stream::StreamFormatter formatter("MakeFt1", "run", 2);
   formatter.info() << "applying TCut: " << filter << std::endl;

   std::string dictFile = m_pars["dict_file"];
   if (dictFile == "DEFAULT") {
      dictFile = st_facilities::Env::appendFileName(dataDir,"FT1variables");
   }

   typedef std::map<std::string, std::string> Ft1Map_t;
   Ft1Map_t ft1Dict;
   ::getFT1Dict(dictFile, ft1Dict);

   dataSubselector::Cuts my_cuts;
   fitsGen::MeritFile merit(rootFile, "MeritTuple", filter);
   fitsGen::Ft1File ft1(fitsFile, merit.nrows());
   
   ::addNeededFields(ft1, ft1Dict);
   
   ft1.header().addHistory("Input merit file: " + rootFile);
   ft1.header().addHistory("Filter string: " + filter);

   EventClassifier eventClass(eventClassifier);
   
   int ncount(0);
   for ( ; merit.itor() != merit.end(); merit.next(), ft1.next()) {
      for (Ft1Map_t::const_iterator variable = ft1Dict.begin();
           variable != ft1Dict.end(); ++variable) {
         ft1[variable->first].set(merit[variable->second]);
      }
//      ft1["event_class"].set(merit.eventType());
      ft1["event_class"].set(eventClass(merit.row()));
      ft1["conversion_type"].set(merit.conversionType());
      ncount++;
   }
   formatter.info() << "number of rows processed: " << ncount << std::endl;
   
   ft1.setNumRows(ncount);
   my_cuts.addGtiCut(merit.gti());
   my_cuts.writeDssKeywords(ft1.header());
   std::ostringstream creator;
   creator << "makeFT1 " << getVersion();
   ft1.setPhduKeyword("CREATOR", creator.str());
   std::string version = m_pars["file_version"];
   ft1.setPhduKeyword("VERSION", version);
   std::string filename(facilities::Util::basename(fitsFile));
   ft1.setPhduKeyword("FILENAME", filename);

   my_cuts.writeGtiExtension(fitsFile);
   st_facilities::FitsUtil::writeChecksums(fitsFile);
   if (st_facilities::Util::fileExists("dummy.root")) {
      std::remove("dummy.root");
   }
}
