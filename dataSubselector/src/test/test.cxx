/**
 * @file test.cxx
 * @brief Tests program for Cuts class.
 * @author J. Chiang
 *
 * $Header$
 */ 

#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <iostream>
#include <stdexcept>

#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "st_facilities/Util.h"

#include "tip/IFileSvc.h"

#include "dataSubselector/Cuts.h"

class DssTests : public CppUnit::TestFixture {

   CPPUNIT_TEST_SUITE(DssTests);

   CPPUNIT_TEST(compareCuts);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void compareCuts();

private:
   std::string m_outfile;
   std::string m_outfile2;

   const tip::Table * m_inputTable;
   tip::Table * m_outputTable;
   tip::Table * m_outputTable2;

   tip::Table::ConstIterator m_inputIt;
   tip::Table::Iterator m_outputIt;
   tip::Table::Iterator m_output2It;
};

void DssTests::setUp() {
   char * root_path = ::getenv("DATASUBSELECTORROOT");
   std::string input_file("input_events.fits");
   if (root_path) {
      input_file = std::string(root_path) + "/Data/" + input_file;
   } else {
      throw std::runtime_error("DATASUBSELECTORROOT not set");
   }
   std::string extension("EVENTS");

   m_outfile = "filtered_events.fits";
   if (st_facilities::Util::fileExists(m_outfile)) {
      std::remove(m_outfile.c_str());
   }

   m_outfile2 = "filtered_events_2.fits";
   if (st_facilities::Util::fileExists(m_outfile2)) {
      std::remove(m_outfile2.c_str());
   }
   
   tip::IFileSvc::instance().createFile(m_outfile, input_file);
   tip::IFileSvc::instance().createFile(m_outfile2, input_file);
   
   m_inputTable = tip::IFileSvc::instance().readTable(input_file, extension);
   
   m_outputTable = tip::IFileSvc::instance().editTable(m_outfile, extension);
   m_outputTable->setNumRecords(m_inputTable->getNumRecords());

   m_outputTable2 = tip::IFileSvc::instance().editTable(m_outfile2, extension);
   m_outputTable2->setNumRecords(m_inputTable->getNumRecords());
   
   m_inputIt = m_inputTable->begin();
   m_outputIt = m_outputTable->begin();
   m_output2It = m_outputTable2->begin();
}

void DssTests::tearDown() {
   delete m_inputTable;
   delete m_outputTable;
   delete m_outputTable2;
}

void DssTests::compareCuts() {
   tip::ConstTableRecord & input = *m_inputIt;
   tip::TableRecord & output = *m_outputIt;
   tip::TableRecord & output2 = *m_output2It;

   dataSubselector::Cuts my_cuts;
      
   my_cuts.addRangeCut("RA", "deg", 83, 93);
   my_cuts.addSkyConeCut(83., 22., 20);
      
   long npts(0);
   long npts2(0);
      
   std::map<std::string, double> params;

   for (; m_inputIt != m_inputTable->end(); ++m_inputIt) {
      if (my_cuts.accept(input)) {
         output = input;
         ++m_outputIt;
         npts++;
      }
      input["RA"].get(params["RA"]);
      input["DEC"].get(params["DEC"]);
      if (my_cuts.accept(params)) {
         output2 = input;
         ++m_output2It;
         npts2++;
      }
      CPPUNIT_ASSERT(my_cuts.accept(input) == my_cuts.accept(params));
   }

   m_outputTable->setNumRecords(npts);
   m_outputTable2->setNumRecords(npts2);
   
   my_cuts.writeDssKeywords(m_outputTable->getHeader());
   my_cuts.writeDssKeywords(m_outputTable2->getHeader());
}

int main() {
   CppUnit::TextTestRunner runner;
   
   runner.addTest(DssTests::suite());
    
   bool result = runner.run();
   if (result) {
      return 0;
   } else {
      return 1;
   }
}
