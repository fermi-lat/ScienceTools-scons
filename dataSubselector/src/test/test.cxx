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
      std::string outfile2("filtered_events_2.fits");
      
      tip::IFileSvc::instance().createFile(output_file, input_file);
      tip::IFileSvc::instance().createFile(outfile2, input_file);

      const tip::Table * inputTable =
         tip::IFileSvc::instance().readTable(input_file, extension);

      tip::Table * outputTable = 
         tip::IFileSvc::instance().editTable(output_file, extension);
      outputTable->setNumRecords(inputTable->getNumRecords());

      tip::Table * outputTable2 = 
         tip::IFileSvc::instance().editTable(outfile2, extension);
      outputTable2->setNumRecords(inputTable->getNumRecords());
      
      tip::Table::ConstIterator inputIt = inputTable->begin();
      tip::Table::Iterator outputIt = outputTable->begin();
      tip::Table::Iterator output2It = outputTable2->begin();
      
      tip::ConstTableRecord & input = *inputIt;
      tip::TableRecord & output = *outputIt;
      tip::TableRecord & output2 = *output2It;

      dataSubselector::Cuts my_cuts;
      
      my_cuts.addRangeCut("RA", "deg", 83, 93);
      my_cuts.addSkyConeCut(83., 22., 20);
      
      long npts(0);
      long npts2(0);
      
      std::map<std::string, double> params;

      for (; inputIt != inputTable->end(); ++inputIt) {
         if (my_cuts.accept(input)) {
            output = input;
            ++outputIt;
            npts++;
         }
         input["RA"].get(params["RA"]);
         input["DEC"].get(params["DEC"]);
         if (my_cuts.accept(params)) {
            output2 = input;
            ++output2It;
            npts2++;
         }
      }

      outputTable->setNumRecords(npts);
      outputTable2->setNumRecords(npts2);
   
      my_cuts.writeDssKeywords(outputTable->getHeader());
      my_cuts.writeDssKeywords(outputTable2->getHeader());

      delete inputTable;
      delete outputTable;
      delete outputTable2;
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
      return 1;
   }
}
