/**
 * @file makeFT1.cxx
 * @brief Convert merit ntuple to FT1 format.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "dataSubselector/Gti.h"
#include "dataSubselector/Cuts.h"

#include "astro/SkyDir.h"

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "fitsGen/Ft1File.h"
#include "fitsGen/MeritFile.h"
#include "fitsGen/Util.h"

using namespace fitsGen;

int main(int iargc, char * argv[]) {
   if (iargc < 3) {
      std::cout << "usage: <merit file> <FT1 file> [<TCut>]"
                << std::endl;
      std::exit(1);
   }

   std::string rootFile(argv[1]);
   std::string fitsFile(argv[2]);

   std::ostringstream filter;
   if (iargc == 4) {
      if (st_facilities::Util::fileExists(argv[3])) {
         std::vector<std::string> lines;
         st_facilities::Util::readLines(argv[3], lines, "#", true);
         for (size_t i = 0; i < lines.size(); i++) {
            filter << lines.at(i);
         }
      } else {
         filter << argv[3];
      }
      std::cout << "applying TCut: " << filter.str() << std::endl;
   }

   dataSubselector::Cuts my_cuts;
   try {
      fitsGen::MeritFile merit(rootFile, "MeritTuple", filter.str());
      fitsGen::Ft1File ft1(fitsFile, merit.nrows());
   
      ft1.header().addHistory("Input merit file: " + rootFile);
      ft1.header().addHistory("Filter string: " + filter.str());

      int ncount(0);
      for ( ; merit.itor() != merit.end(); merit.next(), ft1.next()) {
         ft1["energy"].set(merit["FT1Energy"]);
         double ra = merit["FT1Ra"];
         double dec = merit["FT1Dec"];
         ft1["ra"].set(ra);
         ft1["dec"].set(dec);
         astro::SkyDir dir(ra, dec);
         ft1["l"].set(dir.l());
         ft1["b"].set(dir.b());
         ft1["theta"].set(merit["FT1Theta"]);
         ft1["phi"].set(merit["FT1Phi"]);
         ft1["zenith_angle"].set(merit["FT1ZenithTheta"]);
         ft1["earth_azimuth_angle"].set(merit["FT1EarthAzimuth"]);
         ft1["time"].set(merit["EvtElapsedTime"]);
         ft1["event_id"].set(static_cast<int>(merit["FT1EventId"]));
         if (17 - merit["Tkr1FirstLayer"] < 11.5) { // Front converting
            ft1["event_class"].set(0);
            ft1["conversion_type"].set(0);
         } else { // Back converting
            ft1["event_class"].set(1);
            ft1["conversion_type"].set(1);
         }
         ft1["livetime"].set(merit["FT1Livetime"]);
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

