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

#include "st_facilities/Env.h"
#include "st_facilities/Util.h"

using namespace st_facilities;

class st_facilitiesTests : public CppUnit::TestFixture {

   CPPUNIT_TEST_SUITE(st_facilitiesTests);

   CPPUNIT_TEST(test_dgaus8);
   CPPUNIT_TEST_EXCEPTION(test_Util_file_ok, std::runtime_error);
   CPPUNIT_TEST(test_Util_readLines);
   CPPUNIT_TEST(test_Util_expectedException);
   CPPUNIT_TEST(test_Util_resolve_fits_files);
   CPPUNIT_TEST(test_Env_appendNames);
   CPPUNIT_TEST(test_Env_expandEnvVar);
   CPPUNIT_TEST(test_Env_getDataDir);

   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();

   void test_dgaus8();
   void test_Util_file_ok();
   void test_Util_readLines();
   void test_Util_expectedException();
   void test_Util_resolve_fits_files();
   void test_Env_appendNames();
   void test_Env_expandEnvVar();
   void test_Env_getDataDir();

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

void st_facilitiesTests::test_Env_appendNames() {
   std::string tmp_str;
   std::string::size_type pos;
   std::string seg1 = "seg1-name";
   std::string seg2 = "seg2-name";

   tmp_str = Env::appendPath("", "");
   CPPUNIT_ASSERT(tmp_str.empty());

   tmp_str = Env::appendPath(seg1, "");
   CPPUNIT_ASSERT(tmp_str == seg1);

   tmp_str = Env::appendPath("", seg2);
   CPPUNIT_ASSERT(tmp_str == seg2);

   tmp_str = Env::appendPath(seg1, seg2);
   pos = tmp_str.find(seg1);
   CPPUNIT_ASSERT(0 == pos);
   pos = tmp_str.find(seg2);
   CPPUNIT_ASSERT(tmp_str.size() - seg2.size() == pos);
}

void st_facilitiesTests::test_Env_expandEnvVar() {
   std::string to_expand;
   std::string expanded;

   to_expand = "$STTEST";
   Env::expandEnvVar(to_expand, expanded);
   CPPUNIT_ASSERT("sttest" == expanded);
   expanded.clear();

   to_expand = "${STTEST}";
   Env::expandEnvVar(to_expand, expanded);
   CPPUNIT_ASSERT(expanded == "sttest");
   expanded.clear();

   to_expand = "$(STTEST)";
   Env::expandEnvVar(to_expand, expanded);
   CPPUNIT_ASSERT(expanded == "sttest");
   expanded.clear();

   to_expand = "$(STTEST) filler $STTEST";
   Env::expandEnvVar(to_expand, expanded);
   CPPUNIT_ASSERT(expanded == "sttest filler sttest");
   expanded.clear();

   to_expand = "filler$(STTEST)filler$STTEST";
   Env::expandEnvVar(to_expand, expanded);
   CPPUNIT_ASSERT(expanded == "fillersttestfillersttest");
   expanded.clear();

   to_expand = "$(STTEST)}";
   Env::expandEnvVar(to_expand, expanded);
   CPPUNIT_ASSERT(expanded == "sttest}");
   expanded.clear();

   to_expand = "$STTEST filler";
   Env::expandEnvVar(to_expand, expanded);
   CPPUNIT_ASSERT(expanded == "sttest filler");
   expanded.clear();

   to_expand = "$";
   try {
      Env::expandEnvVar(to_expand, expanded);
      CPPUNIT_ASSERT(false);
   } catch (const std::exception &) {
      CPPUNIT_ASSERT(expanded == to_expand);
   }
   expanded.clear();

   to_expand = "$$";
   try {
      Env::expandEnvVar(to_expand, expanded);
      CPPUNIT_ASSERT(false);
   } catch (const std::exception &) {
      CPPUNIT_ASSERT(expanded == to_expand);
   }
   expanded.clear();

   to_expand = "$STTEST_run_on";
   try {
      Env::expandEnvVar(to_expand, expanded);
      CPPUNIT_ASSERT(false);
   } catch (const std::exception &) {
      CPPUNIT_ASSERT(expanded == to_expand);
   }
   expanded.clear();

   to_expand = "${STTEST:f}";
   try {
      Env::expandEnvVar(to_expand, expanded);
      CPPUNIT_ASSERT(false);
   } catch (const std::exception &) {
      CPPUNIT_ASSERT(expanded == to_expand);
   }
   expanded.clear();

   to_expand = "prefix${STTEST";
   try {
      Env::expandEnvVar(to_expand, expanded);
      CPPUNIT_ASSERT(false);
   } catch (const std::exception &) {
      CPPUNIT_ASSERT(expanded == to_expand);
   }
   expanded.clear();
}

void st_facilitiesTests::test_Env_getDataDir() {
   // For comparison of output, need local and install areas.
   std::string install_area;
   std::string local_area = Env::getEnv("ST_FACILITIESROOT");

   try {
      // For comparison purposes, expand the install area env variable.
      std::string install_area = Env::getEnv("DATADIR");
   } catch (const std::exception &) {
      // That's OK.
   }

   std::string data_dir;
   // First test: an invalid package identifier.
   try {
      data_dir = Env::getDataDir("invalid_pkg");
      // If the above line did not throw an exception, the data_dir must be the install area.
      CPPUNIT_ASSERT(!install_area.empty() && install_area == data_dir);
   } catch (const std::exception &) {
      // Expected only if install area env variable is not set.
      CPPUNIT_ASSERT(install_area.empty());
   }

   data_dir.erase();
   try {
      data_dir = Env::getDataDir("st_facilities");
      // If the above line did not throw an exception, the data_dir must be the local area.
      CPPUNIT_ASSERT(0 == data_dir.find(local_area));
   } catch (const std::exception &) {
      // Unexpected.
      CPPUNIT_ASSERT(false);
   }
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
