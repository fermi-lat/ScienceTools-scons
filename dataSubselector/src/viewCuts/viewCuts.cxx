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
      if (iargc == 2) {
         std::string file(argv[1]);
         dataSubselector::Cuts cuts(file);
         cuts.writeCuts(std::cout);
      } else {
         std::cout << "usage: viewCuts <filename>" << std::endl;
      }
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   }
}
