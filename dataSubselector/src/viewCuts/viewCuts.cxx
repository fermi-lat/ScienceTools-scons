/**
 * @file viewCuts.cxx
 * @brief Simple app to print the DSS keywords in an event extension.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include "dataSubselector/Cuts.h"

int main(int iargc, char * argv[]) {
   try {
      if (iargc == 2 || iargc == 3) {
         std::string file(argv[1]);
         std::string extname;
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
      std::cout << eObj.what() << std::endl;
   }
}
