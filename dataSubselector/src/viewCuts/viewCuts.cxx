/**
 * @file viewCuts.cxx
 * @brief Simple app to print the DSS keywords in an event extension.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include "st_facilities/Util.h"
#include "dataSubselector/Cuts.h"

int main(int iargc, char * argv[]) {
   std::string file;
   std::string extname;
   try {
      if (iargc == 2 || iargc == 3) {
         file = argv[1];
         if (iargc == 3) {
            extname = argv[2];
         } else {
            extname = "EVENTS";
         }
         dataSubselector::Cuts cuts(file, extname, false);
         cuts.writeCuts(std::cout);
      } else {
         std::cout << "usage: viewCuts <filename> [<extname>]" << std::endl;
      }
   } catch (std::exception & eObj) {
      if (st_facilities::Util::expectedException(eObj, "EVENTS")) {
         try {
            dataSubselector::Cuts cuts(file, "", false);
            cuts.writeCuts(std::cout);
         } catch (std::exception & eObj) {
            std::cout << eObj.what() << std::endl;
         }
      } else {
         std::cout << eObj.what() << std::endl;
      }
   }
}
