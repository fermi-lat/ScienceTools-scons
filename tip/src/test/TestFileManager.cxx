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

namespace tip {

  int TestFileManager::test(int status) {
    // Use inherited status to set initial status
    setStatus(status);

    // Test Table's constructors:
    createFileTest();

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
      ReportExpected(msg + " failed");
    }

    // Test creating file with invalid template:
    msg = std::string("creating file IFileSvc_error.fits using a non-existent template\n\t") + data_dir + "non_existent.tpl";
    try {
      IFileSvc::instance().createFile("IFileSvc_error.fits", data_dir + "non_existent.tpl");
      ReportUnexpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportExpected(msg + " failed");
    }

    // Success cases:
    // Test creating file with template:
    msg = std::string("creating file new_ft1.fits using template\n\t") + data_dir + "ft1.tpl";
    try {
      IFileSvc::instance().createFile("IFileSvc_success.fits", data_dir + "ft1.tpl");
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed");
    }

  }

}
