/**
 * @file test.cxx
 * @brief Tests program for Cuts class.
 * @author J. Chiang
 *
 * $Header$
 */ 

#include <iostream>
#include <stdexcept>

#include "tip/IFileSvc.h"

#include "dataSubselector/Cuts.h"

int main() {
   try {
      char * root_path = ::getenv("DATASUBSELECTORROOT");
      std::string input_file("input_events.fits");
      if (root_path) {
         input_file = std::string(root_path) + "/Data/" + input_file;
      } else {
         throw std::runtime_error("DATASUBSELECTORROOT not set");
      }
      std::string extension("EVENTS");
      std::string output_file("filtered_events.fits");
      
      tip::IFileSvc::instance().createFile(output_file, input_file);
      const tip::Table * inputTable =
         tip::IFileSvc::instance().readTable(input_file, extension);
      tip::Table * outputTable = 
         tip::IFileSvc::instance().editTable(output_file, extension);
      outputTable->setNumRecords(inputTable->getNumRecords());
      
      tip::Table::ConstIterator inputIt = inputTable->begin();
      tip::Table::Iterator outputIt = outputTable->begin();
      
      tip::ConstTableRecord & input = *inputIt;
      tip::TableRecord & output = *outputIt;

      dataSubselector::Cuts my_cuts;
      
      my_cuts.addRangeCut("RA", "deg", 83, 93);
      my_cuts.addSkyConeCut(83., 22., 20);
      
      long npts(0);
      
      for (; inputIt != inputTable->end(); ++inputIt) {
         if (my_cuts.accept(input)) {
            output = input;
            ++outputIt;
            npts++;
         }
      }

      outputTable->setNumRecords(npts);
   
      my_cuts.writeDssKeywords(outputTable->getHeader());

      delete inputTable;
      delete outputTable;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
      return 1;
   }
}
