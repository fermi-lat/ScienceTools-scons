/**
 * @file test.cxx
 * @brief Scaffolding for EventClassifier code.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdexcept>

#include "facilities/commonUtilities.h"

#include "embed_python/Module.h"

#include "fitsGen/EventClassifier.h"
#include "fitsGen/MeritFile.h"
#include "fitsGen/MeritFile2.h"
#include "fitsGen/XmlEventClassifier.h"

std::string datapath(const std::string & basename) {
   return facilities::commonUtilities::joinPath(
      facilities::commonUtilities::getDataPath("fitsGen"), 
      basename);
}

void run(fitsGen::EventClassifier & eventClass,
         double ctbgam, double ctbcore, int tkrLayer=5) {
   std::map<std::string, double> my_row;
   my_row["CTBSummedCTBGAM"] = ctbgam;
   my_row["CTBCORE"] = ctbcore;
   my_row["Tkr1FirstLayer"] = tkrLayer;
   my_row["CTBBestEnergyProb"] = 0.2;
   std::cout << eventClass(my_row) << std::endl;
}

int xml_classifier() {
   std::string merit_file(datapath("xml_test_merit.root"));
   std::string xml_file(datapath("EvtClassDefs_Test.xml"));
   
   fitsGen::XmlEventClassifier foo(xml_file, merit_file);

   unsigned int run[3] = {239559565, 239559565, 239572736};
   unsigned int event_id[3] = {4851437, 10436210, 76017};

   std::cout << foo.passVersion() << std::endl;

   for (size_t i(0); i < 3; i++) {
      unsigned int evtclass = foo(run[i], event_id[i]);
      std::cout << run[i] << "  "
                << event_id[i] << "  "
                << evtclass << "  "
                << foo.is_class_member(run[i], event_id[i], 5) << "  "
                << std::endl;
   }
   return 0;
}

int test_MeritFile2() {
   std::string infile(datapath("xml_test_merit.root"));
   std::string treename("MeritTuple");
   std::string filter("EvtEnergyCorr>1e5");
   fitsGen::MeritFile merit(infile, treename, filter);
   fitsGen::MeritFile2 merit2(infile, treename, filter);
   assert(merit.nrows() == merit2.nrows());
   assert(merit.tstart() == merit2.tstart());
   assert(merit.tstop() == merit2.tstop());
   std::cout << std::setprecision(13);
   std::cout << merit.tstart() << "  " << merit2.tstart() << std::endl;
   std::cout << merit.tstop() << "  " << merit2.tstop() << std::endl;
   for ( ; merit2.index() != merit2.nrows(); merit.next(), merit2.next()) {
      assert(merit["EvtElapsedTime"] == merit2["EvtElapsedTime"]);
      assert(merit["EvtEnergyCorr"] == merit2["EvtEnergyCorr"]);
      assert(merit["TkrNumTracks"] == merit2["TkrNumTracks"]);
      assert(merit["EvtEventId"] == merit2["EvtEventId"]);
      assert(merit.conversionType() == merit2.conversionType());
//      std::cout << merit["EvtEnergyCorr"] << std::endl;
   }
   try {
      merit2["EvtEnrgyCore"];
   } catch(std::runtime_error & eObj) {
//      std::cout << eObj.what() << std::endl;
   }
}

int main() {
   try {
      fitsGen::EventClassifier foo("Pass4_Classifier");
      run(foo, 0.6, 0.8, 10);
      run(foo, 0.6, 0.7, 5);
      run(foo, 0.6, 0.3);
      run(foo, 0.3, 0.8);
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   }
   try {
      xml_classifier();
   } catch (std::exception & eObj) {
      std::cout << eObj.what() << std::endl;
   }
   test_MeritFile2();
}
