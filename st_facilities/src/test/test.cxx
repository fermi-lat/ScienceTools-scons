/**
 * @file test.cxx
 * @brief Test program for st_facilities
 * @author J. Chiang
 *
 * $Header$
 */

#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "st_facilities/dgaus8.h"
#include "PowerLaw.h"

#include "st_facilities/Util.h"

using namespace st_facilities;

class st_facilitiesTests : public CppUnit::TestFixture {

   CPPUNIT_TEST_SUITE(st_facilitiesTests);

   CPPUNIT_TEST(test_dgaus8);
   CPPUNIT_TEST_EXCEPTION(test_Util_file_ok, std::runtime_error);
   CPPUNIT_TEST(test_Util_readLines);
   CPPUNIT_TEST(test_Util_expectedException);
   CPPUNIT_TEST(test_Util_resolve_fits_files);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void test_dgaus8();
   void test_Util_file_ok();
   void test_Util_readLines();
   void test_Util_expectedException();
   void test_Util_resolve_fits_files();

private:

   std::string m_filename;

};

void st_facilitiesTests::setUp() {
   m_filename = "test_file.txt";
   std::ofstream file(m_filename.c_str());
   file << "line 0\n" 
        << "line 1\n" 
        << "line 2\n" 
        << "#line 3\n"
        << " "
        << "\n";
   file.close();
}

void st_facilitiesTests::tearDown() {
   std::remove(m_filename.c_str());
}

PowerLaw powerLaw(1., 2.);

double power_law(double * x) {
   return powerLaw(*x);
}

void st_facilitiesTests::test_dgaus8() {
   double xmin(0);
   double xmax(4.);
   double err(1e-5);
   double result(0);
   long ierr;
   dgaus8_(&power_law, &xmin, &xmax, &err, &result, &ierr);
   double true_value;
   if (powerLaw.index() != 1.) {
      true_value = powerLaw.prefactor()*(pow(xmax, powerLaw.index() + 1.) 
                                         - pow(xmin, powerLaw.index() + 1.))
                                         /(powerLaw.index() + 1.);
   } else {
      true_value = powerLaw.prefactor()*log(xmax/xmin);
   }
   double tol(1e-4);
   CPPUNIT_ASSERT(std::fabs((result - true_value)/true_value) < tol);
}

void st_facilitiesTests::test_Util_file_ok() {
   std::string filename("foo");
   std::remove(filename.c_str());
   Util::file_ok(filename);
}

void st_facilitiesTests::test_Util_readLines() {
   std::vector<std::string> lines;
   Util::readLines(m_filename, lines);
   CPPUNIT_ASSERT(lines.size() == 3);
   Util::readLines(m_filename, lines, "");
   CPPUNIT_ASSERT(lines.size() == 4);
}

void st_facilitiesTests::test_Util_expectedException() {
   try {
      test_Util_file_ok();
   } catch (std::exception & eObj) {
      CPPUNIT_ASSERT(Util::expectedException(eObj, "File not found"));
   }
}

void st_facilitiesTests::test_Util_resolve_fits_files() {
   std::vector<std::string> lines;
   Util::resolve_fits_files(m_filename, lines);
   CPPUNIT_ASSERT(lines.size() == 3);
}
   
int main() {
   CppUnit::TextTestRunner runner;
   
   runner.addTest(st_facilitiesTests::suite());
    
   bool result = runner.run();
   if (result) {
      return 0;
   } else {
      return 1;
   }
}
