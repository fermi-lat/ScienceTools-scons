/** \file TestFileManager.cxx
    \brief Definition of class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/

#include <cstdlib>
#include <iostream>

#include "FitsFileManager.h"
#include "RootExtensionData.h"
#include "TestFileManager.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/tip_types.h"

namespace tip {

  int TestFileManager::test(int status) {
    // Use inherited status to set initial status.
    setStatus(status);

    // Test file creation.
    createFileTest();

    // Test opening table read-only:
    readTableTest();
    
    return getStatus();
  }

  void TestFileManager::createFileTest() {
    std::string msg;

    // Find test data directory:
    std::string data_dir = getDataDir();

    // Failure cases:
    msg = "creating file in an invalid location /invalid/directory/file";
    try {
      IFileSvc::instance().createFile("/invalid/directory/file");
      ReportUnexpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportExpected(msg + " failed", x);
    }

    // Test creating file with invalid template:
    msg = std::string("creating file IFileSvc_error.fits using a non-existent template\n\t") + data_dir + "non_existent.tpl";
    try {
      IFileSvc::instance().createFile("IFileSvc_error.fits", data_dir + "non_existent.tpl");
      ReportUnexpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportExpected(msg + " failed", x);
    }

    // Success cases:
    // Test creating file with template:
    msg = std::string("creating file new_ft1.fits using template\n\t") + data_dir + "ft1.tpl";
    try {
      IFileSvc::instance().createFile("IFileSvc_success.fits", data_dir + "ft1.tpl");
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

  }

  void TestFileManager::readTableTest() {
    std::string msg;
    const Table * table = 0;

    // Find test data directory:
    std::string data_dir = getDataDir();

    // Test opening file read-only:
    msg = std::string("TestFileManager::readTableTest opening read-only extension SPECTRUM of file ") + data_dir + "a1.pha";
    try {
      table = IFileSvc::instance().readTable(data_dir + "a1.pha", "SPECTRUM", "#row > 50 && #row <= 100");
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed");
    }

    // Basic sanity check only: is the number of records 50, as the filter should have selected?
    if (0 != table) {
      Index_t num_rec = -1;
      try {
        num_rec = table->getNumRecords();
        msg = std::string("with filtering expression, number of records in table is ") + toString(num_rec);

        if (50 == num_rec) ReportExpected(msg + ", as expected");
        else ReportUnexpected(msg + ", not 50, as expected");
      } catch(const TipException & x) {
        ReportUnexpected("TestFileManager::readTableTest: getNumRecords failed");
      }
    }

    delete table;
  }

}
