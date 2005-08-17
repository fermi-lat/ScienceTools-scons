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

#include <cmath>

#include <iostream>
#include <stdexcept>

#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "st_facilities/FitsUtil.h"
#include "st_facilities/Util.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include "dataSubselector/Cuts.h"
#include "dataSubselector/Gti.h"

class DssTests : public CppUnit::TestFixture {

   CPPUNIT_TEST_SUITE(DssTests);

   CPPUNIT_TEST(compareGtis);
   CPPUNIT_TEST(updateGti);
   CPPUNIT_TEST(combineGtis);
   CPPUNIT_TEST(compareCuts);
   CPPUNIT_TEST(compareCutsWithoutGtis);
   CPPUNIT_TEST(mergeGtis);
   CPPUNIT_TEST(compareUnorderedCuts);
   CPPUNIT_TEST(cutsConstructor);
   CPPUNIT_TEST(test_SkyCone);
   CPPUNIT_TEST(test_DssFormatting);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void compareGtis();
   void updateGti();
   void combineGtis();
   void compareCuts();
   void compareCutsWithoutGtis();
   void mergeGtis();
   void compareUnorderedCuts();
   void cutsConstructor();
   void test_SkyCone();
   void test_DssFormatting();

private:

   std::string m_infile;
   std::string m_outfile;
   std::string m_outfile2;

   const tip::Table * m_inputTable;
   tip::Table * m_outputTable;
   tip::Table * m_outputTable2;

   tip::Table::ConstIterator m_inputIt;
   tip::Table::Iterator m_outputIt;
   tip::Table::Iterator m_output2It;

};

#define ASSERT_EQUALS(X, Y) CPPUNIT_ASSERT(std::fabs( (X - Y)/Y ) < 1e-4)

void DssTests::setUp() {
   char * root_path = ::getenv("DATASUBSELECTORROOT");
   m_infile = "input_events.fits";
   if (root_path) {
      m_infile = std::string(root_path) + "/Data/" + m_infile;
   } else {
      throw std::runtime_error("DATASUBSELECTORROOT not set");
   }
   m_outfile = "filtered_events.fits";
   m_outfile2 = "filtered_events_2.fits";
}

void DssTests::tearDown() {
}

void DssTests::compareGtis() {
   const tip::Table * gtiTable = 
      tip::IFileSvc::instance().readTable(m_infile, "GTI");

   dataSubselector::Gti gti1(*gtiTable);
   dataSubselector::Gti gti2(m_infile);

   CPPUNIT_ASSERT(!(gti1 != gti2));

   gti1.insertInterval(0, 10.);

   CPPUNIT_ASSERT(gti1 != gti2);
}

void DssTests::updateGti() {
   dataSubselector::Gti gti;
   gti.insertInterval(0, 1000.);
   gti.insertInterval(1500, 2000.);
   dataSubselector::Gti new_gti = gti.applyTimeRangeCut(500., 1750.);

   double expected_values[2][2] = {{500, 1000}, {1500, 1750}};
   std::vector< std::pair<double, double> >::const_iterator interval;
   int i(0);
   for (interval = new_gti.begin();
        interval != new_gti.end(); ++interval, i++) {
      ASSERT_EQUALS(interval->first, expected_values[i][0]);
      ASSERT_EQUALS(interval->second, expected_values[i][1]);
   }
}

void DssTests::cutsConstructor() {
   dataSubselector::Cuts my_cuts(m_infile);

   CPPUNIT_ASSERT(my_cuts.size() == 2);

   std::map<std::string, double> params;
   params["ENERGY"] = 20.;
   CPPUNIT_ASSERT(!my_cuts.accept(params));
   params["ENERGY"] = 30.;
   CPPUNIT_ASSERT(my_cuts.accept(params));
   params["ENERGY"] = 100.;
   CPPUNIT_ASSERT(my_cuts.accept(params));
   params["ENERGY"] = 2e5;
   CPPUNIT_ASSERT(my_cuts.accept(params));
   params["ENERGY"] = 2.1e5;
   CPPUNIT_ASSERT(!my_cuts.accept(params));

   params["ENERGY"] = 100.;
   params["TIME"] = 100.;
   CPPUNIT_ASSERT(my_cuts.accept(params));
   params["TIME"] = 9e4;
   CPPUNIT_ASSERT(!my_cuts.accept(params));
}

void DssTests::test_SkyCone() {
   dataSubselector::Cuts my_cuts(m_outfile);
   
   std::map<std::string, double> params;

   params["RA"] = 85;
   params["DEC"] = 22;
   CPPUNIT_ASSERT(my_cuts.accept(params));

   params["DEC"] = -40;
   CPPUNIT_ASSERT(!my_cuts.accept(params));
}

void DssTests::combineGtis() {
   dataSubselector::Gti gti1, gti2;

   gti1.insertInterval(200., 300.);
   gti2.insertInterval(500., 700.);

   dataSubselector::Gti new_gti, test_gti;
   test_gti.insertInterval(200., 300.);
   test_gti.insertInterval(500., 700.);

   new_gti = gti1 | gti2;
   CPPUNIT_ASSERT(new_gti.getNumIntervals() == 2);
   CPPUNIT_ASSERT(!(new_gti != test_gti));

   new_gti = gti2 | gti1;
   CPPUNIT_ASSERT(new_gti.getNumIntervals() == 2);
   CPPUNIT_ASSERT(!(new_gti != test_gti));

   dataSubselector::Gti gti3, gti4, test_gti34;
   gti3.insertInterval(200., 500.);
   gti4.insertInterval(300., 700.);
   test_gti34.insertInterval(200., 700.);

   new_gti = gti3 | gti4;
   CPPUNIT_ASSERT(new_gti.getNumIntervals() == 1);
   CPPUNIT_ASSERT(!(new_gti != test_gti34));
   
   new_gti = gti4 | gti3;
   CPPUNIT_ASSERT(new_gti.getNumIntervals() == 1);
   CPPUNIT_ASSERT(!(new_gti != test_gti34));

   dataSubselector::Gti gti5, gti6;
   gti5.insertInterval(200., 700.);
   gti6.insertInterval(300., 500.);
   
   new_gti = gti5 | gti6;
   CPPUNIT_ASSERT(new_gti.getNumIntervals() == 1);
   CPPUNIT_ASSERT(!(new_gti != test_gti34));
   
   new_gti = gti6 | gti5;
   CPPUNIT_ASSERT(new_gti.getNumIntervals() == 1);
   CPPUNIT_ASSERT(!(new_gti != test_gti34));
}

void DssTests::compareCuts() {
   std::string extension("EVENTS");

   if (st_facilities::Util::fileExists(m_outfile)) {
      std::remove(m_outfile.c_str());
   }
   if (st_facilities::Util::fileExists(m_outfile2)) {
      std::remove(m_outfile2.c_str());
   }
   
   tip::IFileSvc::instance().createFile(m_outfile, m_infile);
   tip::IFileSvc::instance().createFile(m_outfile2, m_infile);
   
   m_inputTable = tip::IFileSvc::instance().readTable(m_infile, extension);
   
   m_outputTable = tip::IFileSvc::instance().editTable(m_outfile, extension);
   m_outputTable->setNumRecords(m_inputTable->getNumRecords());

   m_outputTable2 = tip::IFileSvc::instance().editTable(m_outfile2, extension);
   m_outputTable2->setNumRecords(m_inputTable->getNumRecords());
   
   m_inputIt = m_inputTable->begin();
   m_outputIt = m_outputTable->begin();
   m_output2It = m_outputTable2->begin();

   tip::ConstTableRecord & input = *m_inputIt;
   tip::TableRecord & output = *m_outputIt;
   tip::TableRecord & output2 = *m_output2It;

   dataSubselector::Cuts my_cuts(m_infile);
      
   my_cuts.addRangeCut("RA", "deg", 83, 93);
   my_cuts.addSkyConeCut(83., 22., 20);
   my_cuts.addRangeCut("CALIB_VERSION", "dimensionless", 1, 1,
                       dataSubselector::RangeCut::CLOSED, 1);
      
   long npts(0);
   long npts2(0);
      
   std::map<std::string, double> params;

   for (; m_inputIt != m_inputTable->end(); ++m_inputIt) {
      if (my_cuts.accept(input)) {
         output = input;
         ++m_outputIt;
         npts++;
      }
      input["ENERGY"].get(params["ENERGY"]);
      input["TIME"].get(params["TIME"]);
      input["RA"].get(params["RA"]);
      input["DEC"].get(params["DEC"]);
      params["CALIB_VERSION[1]"] = 1;
      if (my_cuts.accept(params)) {
         output2 = input;
         ++m_output2It;
         npts2++;
      }
      CPPUNIT_ASSERT(my_cuts.accept(input) == my_cuts.accept(params));

      if (my_cuts.accept(input)) {
         params["CALIB_VERSION[1]"] = 0;
         CPPUNIT_ASSERT(my_cuts.accept(input) != my_cuts.accept(params));
      }
   }

   m_outputTable->setNumRecords(npts);
   m_outputTable2->setNumRecords(npts2);
   
   my_cuts.writeDssKeywords(m_outputTable->getHeader());
   my_cuts.writeDssKeywords(m_outputTable2->getHeader());

   delete m_inputTable;
   delete m_outputTable;
   delete m_outputTable2;

   st_facilities::FitsUtil::writeChecksums(m_outfile);
   st_facilities::FitsUtil::writeChecksums(m_outfile2);

   dataSubselector::Cuts cuts1(m_outfile);
   dataSubselector::Cuts cuts2(m_outfile2);
   CPPUNIT_ASSERT(cuts1 == cuts2);

   dataSubselector::Cuts cuts(m_infile);
   CPPUNIT_ASSERT(!(cuts == cuts1));
}

void DssTests::compareCutsWithoutGtis() {
   dataSubselector::Cuts cuts1;
   cuts1.addRangeCut("Energy", "MeV", 30., 2e5);
   cuts1.addSkyConeCut(83.57, 22.01, 20);

   dataSubselector::Cuts cuts2(cuts1);

   dataSubselector::Gti gti1, gti2;
   gti1.insertInterval(100., 500.);
   gti2.insertInterval(300., 700.);

   cuts1.addGtiCut(gti1);
   cuts2.addGtiCut(gti2);

   CPPUNIT_ASSERT(!(cuts1 == cuts2));
   CPPUNIT_ASSERT(cuts1.compareWithoutGtis(cuts2));
}

void DssTests::mergeGtis() {
   dataSubselector::Cuts cuts1;
   cuts1.addRangeCut("Energy", "MeV", 30., 2e5);
   cuts1.addSkyConeCut(83.57, 22.01, 20);

   dataSubselector::Cuts cuts2;
   
   cuts2 = cuts1;

   CPPUNIT_ASSERT(cuts2 == cuts1);

   dataSubselector::Gti gti1, gti2;
   gti1.insertInterval(100., 500.);
   gti2.insertInterval(300., 700.);

   cuts1.addGtiCut(gti1);
   cuts2.addGtiCut(gti2);

   std::vector<dataSubselector::Cuts> my_cuts;
   my_cuts.push_back(cuts1);
   my_cuts.push_back(cuts2);

   dataSubselector::Cuts newCuts = dataSubselector::Cuts::mergeGtis(my_cuts);

   CPPUNIT_ASSERT(newCuts != cuts1);
   CPPUNIT_ASSERT(newCuts != cuts2);
   CPPUNIT_ASSERT(newCuts.compareWithoutGtis(cuts1));
   CPPUNIT_ASSERT(newCuts.compareWithoutGtis(cuts2));

   std::vector<const dataSubselector::GtiCut *> gtiCuts;
   newCuts.getGtiCuts(gtiCuts);

   CPPUNIT_ASSERT(gtiCuts.size() == 1);

   dataSubselector::Gti mergedGti;
   mergedGti.insertInterval(100., 700.);

   CPPUNIT_ASSERT(!(gtiCuts.at(0)->gti() != mergedGti));
}

void DssTests::compareUnorderedCuts() {
   dataSubselector::Cuts cuts0;
   dataSubselector::Cuts cuts1;

   cuts0.addRangeCut("RA", "deg", 83, 93);
   cuts0.addSkyConeCut(83., 22., 20);
   cuts0.addRangeCut("CALIB_VERSION", "dimensionless", 1, 1,
                     dataSubselector::RangeCut::CLOSED, 1);
      
   cuts1.addRangeCut("CALIB_VERSION", "dimensionless", 1, 1,
                     dataSubselector::RangeCut::CLOSED, 1);
   cuts1.addSkyConeCut(83., 22., 20);
   cuts1.addRangeCut("RA", "deg", 83, 93);

   CPPUNIT_ASSERT(cuts0 == cuts1);
}

void DssTests::test_DssFormatting() {
   std::string testfile1("dss_test1.fits");
   std::string testfile2("dss_test2.fits");
   if (st_facilities::Util::fileExists(testfile1)) {
      std::remove(testfile1.c_str());
   }
   if (st_facilities::Util::fileExists(testfile2)) {
      std::remove(testfile2.c_str());
   }
   tip::IFileSvc::instance().createFile(testfile1, m_infile);
   tip::IFileSvc::instance().createFile(testfile2, m_infile);

   dataSubselector::Cuts cuts1;
   cuts1.addRangeCut("TIME", "s", -1.5046090110e7, 505910.);
   tip::Table * table1 =
      tip::IFileSvc::instance().editTable(testfile1, "EVENTS");
   cuts1.writeDssKeywords(table1->getHeader());
   delete table1;

   tip::Table * table2 =
      tip::IFileSvc::instance().editTable(testfile2, "EVENTS");
   cuts1.writeDssKeywords(table2->getHeader());
   delete table2;

   table1 = tip::IFileSvc::instance().editTable(testfile1, "EVENTS");
   table2 = tip::IFileSvc::instance().editTable(testfile2, "EVENTS");

   const tip::Header & header1 = table1->getHeader();
   const tip::Header & header2 = table2->getHeader();

   std::string value1, value2;
   header1["DSVAL1"].get(value1);
   header2["DSVAL1"].get(value2);
   CPPUNIT_ASSERT(value1 == value2);

   delete table1;
   delete table2;
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
