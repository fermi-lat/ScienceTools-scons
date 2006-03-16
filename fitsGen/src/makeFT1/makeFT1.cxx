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

#include "fitsGen/Ft1File.h"
#include "fitsGen/MeritFile.h"
#include "fitsGen/Util.h"

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
         facilities::Util::stringTokenize(lines.at(i), " ", tokens);
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

int main(int iargc, char * argv[]) {
   if (iargc < 3) {
      std::cout << "usage: <merit file> <FT1 file> "
                << "[<TCut> [<Merit-to-FT1 dictionary file>]]\n";
      std::exit(1);
   }
   
   std::string rootFile(argv[1]);
   std::string fitsFile(argv[2]);

   std::string dataDir(st_facilities::Env::getDataDir("fitsGen"));
   std::string defaultFilter =
      st_facilities::Env::appendFileName(dataDir, "std_cuts");

   std::string filter(filterString(defaultFilter));

   if (iargc >= 4) {
      if (st_facilities::Util::fileExists(argv[3])) {
         filter = filterString(argv[3]);
      } else {
         filter = argv[3];
      }
   }
   std::cout << "applying TCut: " << filter << std::endl;

   std::string dictFile = 
      st_facilities::Env::appendFileName(dataDir,"FT1variables");
   if (iargc == 5) {
      dictFile = argv[4];
   }
   typedef std::map<std::string, std::string> Ft1Map_t;
   Ft1Map_t ft1Dict;
   ::getFT1Dict(dictFile, ft1Dict);

   dataSubselector::Cuts my_cuts;
   try {
      fitsGen::MeritFile merit(rootFile, "MeritTuple", filter);
      fitsGen::Ft1File ft1(fitsFile, merit.nrows());

      ::addNeededFields(ft1, ft1Dict);
   
      ft1.header().addHistory("Input merit file: " + rootFile);
      ft1.header().addHistory("Filter string: " + filter);

      int ncount(0);
      for ( ; merit.itor() != merit.end(); merit.next(), ft1.next()) {
         for (Ft1Map_t::const_iterator variable = ft1Dict.begin();
              variable != ft1Dict.end(); ++variable) {
            ft1[variable->first].set(merit[variable->second]);
         }
         ft1["event_class"].set(merit.eventType());
         ft1["conversion_type"].set(merit.conversionType());
         ncount++;
      }
      std::cout << "number of rows processed: " << ncount << std::endl;

      ft1.setNumRows(ncount);
      my_cuts.addGtiCut(merit.gti());
      my_cuts.writeDssKeywords(ft1.header());
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
      return 1;
   }
   my_cuts.writeGtiExtension(fitsFile);
   st_facilities::FitsUtil::writeChecksums(fitsFile);
   if (st_facilities::Util::fileExists("dummy.root")) {
      std::remove("dummy.root");
   }
}
