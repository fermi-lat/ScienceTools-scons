/**
 * @file viewCuts.cxx
 *
 */

#include <iostream>
#include "dataSubselector/Cuts.h"

int main(int iargc, char * argv[]) {
   if (iargc == 2) {
      std::string file(argv[1]);
      dataSubselector::Cuts cuts(file);
      cuts.writeCuts(std::cout);
   } else {
      std::cout << "usage: viewCuts <filename>" << std::endl;
   }
}
