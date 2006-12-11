/**
 * @file test.cxx
 * @brief Scaffolding for EventClassifier code.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <stdexcept>

#include "embed_python/Module.h"

#include "fitsGen/EventClassifier.h"

void run(fitsGen::EventClassifier & eventClass,
         double ctbgam, double ctbcore) {
   std::map<std::string, double> my_row;
   my_row["CTBSummedCTBGAM"] = ctbgam;
   my_row["CTBCORE"] = ctbcore;
   std::cout << eventClass(my_row) << std::endl;
}

int main() {
   try {
      fitsGen::EventClassifier foo("EventClassifier");
      run(foo, 0.5, 0.8);
      run(foo, 0.5, 0.7);
      run(foo, 0.5, 0.3);
      run(foo, 0.3, 0.8);
      run(foo, 0.05, 0.8);
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   }
}
