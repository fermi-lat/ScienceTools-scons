/** 
 * @file dataSubselector.cxx
 * @brief Filter FT1 data.
 * @author J. Chiang
 *
 *  $Header$
 */

#include "facilities/Util.h"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include "tip/IFileSvc.h"

#include "dataSubselector/CutParameters.h"

/**
 * @class DataFilter
 * @author J. Chiang
 *
 * $Header$
 */

class DataFilter : public st_app::StApp {
public:
   DataFilter() : st_app::StApp(), 
                  m_pars(st_app::StApp::getParGroup("dataSubselector")) {
      try {
         m_pars.Prompt();
         m_pars.Save();
      } catch (std::exception & eObj) {
         std::cerr << eObj.what() << std::endl;
         std::exit(1);
      } catch (...) {
         std::cerr << "Caught unknown exception in DataFilter constructor." 
                   << std::endl;
         std::exit(1);
      }
   }

   virtual ~DataFilter() throw() {
      try {
      } catch (std::exception &eObj) {
         std::cerr << eObj.what() << std::endl;
      } catch (...) {
      }
   }

   virtual void run();

private:

   st_app::AppParGroup & m_pars;

};

st_app::StAppFactory<DataFilter> myAppFactory;

void DataFilter::run() {

   CutParameters cuts(m_pars);

   std::string inputFile = m_pars["input_file"];
   facilities::Util::expandEnvVar(&inputFile);
   tip::Table * inputTable 
      = tip::IFileSvc::instance().editTable(inputFile, "events",
                                            cuts.fitsQueryString());
   
   std::string outputFile = m_pars["output_file"];
   facilities::Util::expandEnvVar(&outputFile);
   tip::IFileSvc::instance().createFile(outputFile, inputFile);
   tip::Table * outputTable 
      = tip::IFileSvc::instance().editTable(outputFile, "events");

   outputTable->setNumRecords(inputTable->getNumRecords());

   tip::Table::Iterator inputIt = inputTable->begin();
   tip::Table::Iterator outputIt = outputTable->begin();

   tip::TableRecord & input = *inputIt;
   tip::Table::Record & output = *outputIt;

   const std::vector<std::string> & columnNames = inputTable->getValidFields();

   for (; inputIt != inputTable->end(); ++inputIt, ++outputIt) {
// Ideally, one would like to be able to use copy assignment here.
//      output = input;

      for (std::vector<std::string>::const_iterator name = columnNames.begin();
           name != columnNames.end(); ++name) {
// Still would be nice if copy assignment worked between "Cells", but alas...
//         output[*name] = input[*name];
//
// How about if one can use set methods directly?....nope.
//         output[*name].set(input[*name]);

// So we are forced to use an intermediate variable to store the data
// and treat vectors as a special case.
         try {
            double value;
            input[*name].get(value);
            output[*name].set(value);
         } catch (tip::TipException &) {
// Here we must rely on our knowledge that the only vectors in FT1 are
// calib_version and conversion_point (would be nice if one could type
// check here rather than having to know calib_version comprises shorts
// and conversion_point floats).
            if (*name == "calib_version") {
               tip::Table::Vector<short> calibVersion_in = input[*name];
               tip::Table::Vector<short> calibVersion_out = output[*name];
               int npts = calibVersion_in.getNumElements();
               for (int i = 0; i < npts; i++) {
                  short ivalue = calibVersion_in[i];
                  calibVersion_out[i] = ivalue;
               }
            } else if (*name == "conversion_point") {
               tip::Table::Vector<float> conversion_point_in = input[*name];
               tip::Table::Vector<float> conversion_point_out = output[*name];
               int npts = conversion_point_in.getNumElements();
               for (int i = 0; i < npts; i++) {
                  short ivalue = conversion_point_in[i];
                  conversion_point_out[i] = ivalue;
               }
            } else {
               throw;
            }
         }
      }
   }

   cuts.addDataSubspaceKeywords(outputTable);

   delete inputTable;
   delete outputTable;
   std::cout << "Done." << std::endl;
}
