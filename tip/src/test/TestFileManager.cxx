/** \file TestFileManager.cxx
    \brief Definition of class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <iostream>

#include "FitsFileManager.h"
#include "RootTable.h"
#include "TestFileManager.h"
#include "tip/Extension.h"
#include "tip/FileSummary.h"
#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Table.h"
#include "tip/tip_types.h"

namespace tip {

  int TestFileManager::test(int status) {
    // Use inherited status to set initial status.
    setStatus(status);

    // Test file creation.
    createFileTest();

    // Test opening extensions generically read-only:
    editExtensionTest();
    
    // Test opening extensions generically read-only:
    readExtensionTest();
    
    // Test opening table read-only:
    readTableTest();

    // Test fileStatus method.
    fileStatusTest();

    // Test updateKeywords.
    updateKeywordsTest();

    // Test image creation.
    appendImageTest();

    // Test table creation.
    appendTableTest();

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
    msg = std::string("creating file IFileSvc_success.fits using template\n\t") + data_dir + "ft1.tpl";
    try {
      IFileSvc::instance().createFile("IFileSvc_success.fits", data_dir + "ft1.tpl");
      std::auto_ptr<const Table> table(IFileSvc::instance().readTable("IFileSvc_success.fits", "EVENTS"));
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

    // Test clobbering file with template:
    msg = std::string("creating file IFileSvc_success.fits using template\n\t") + data_dir + "ft1.tpl";
    try {
      IFileSvc::instance().createFile("IFileSvc_success.fits", data_dir + "ft1.tpl");
      std::auto_ptr<const Table> table(IFileSvc::instance().readTable("IFileSvc_success.fits", "EVENTS"));
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

    // Test creating file without template:
    msg = std::string("creating file new.fits using no template");
    try {
      IFileSvc::instance().createFile("IFileSvc_no_template.fits", "");
      std::auto_ptr<const Image> image(IFileSvc::instance().readImage("IFileSvc_no_template.fits", "PRIMARY"));
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

    // More failure cases:
    msg = "re-creating file IFileSvc_success.fits with clobber false using template\n\t" + data_dir + "ft1.tpl";
    try {
      IFileSvc::instance().createFile("IFileSvc_success.fits", data_dir + "ft1.tpl", false);
      ReportUnexpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportExpected(msg + " failed", x);
    }

    msg = "re-creating file IFileSvc_success.fits with clobber false without template";
    try {
      IFileSvc::instance().createFile("IFileSvc_no_template.fits", "", false);
      ReportUnexpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportExpected(msg + " failed", x);
    }

  }

  void TestFileManager::editExtensionTest() {
    std::string msg;
    Extension * ext = 0;

    // Find test data directory:
    std::string data_dir = getDataDir();

    // Test opening extension read-write:
    msg = std::string("TestFileManager::editExtensionTest opening read-write extension SPECTRUM of file ") + data_dir + "a1.pha";
    try {
      ext = IFileSvc::instance().editExtension(data_dir + "a1.pha", "SPECTRUM", "#row > 50 && #row <= 100");
      ReportExpected(msg + " succeeded");

      Table * table = dynamic_cast<Table *>(ext);
      if (0 == table) ReportUnexpected(msg + ": extension is not a table");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed");
    }

    delete ext;
  }

  void TestFileManager::readExtensionTest() {
    std::string msg;
    const Extension * ext = 0;

    // Find test data directory:
    std::string data_dir = getDataDir();

    // Test opening extension read-only:
    msg = std::string("TestFileManager::readExtensionTest opening read-only extension SPECTRUM of file ") + data_dir + "a1.pha";
    try {
      ext = IFileSvc::instance().readExtension(data_dir + "a1.pha", "SPECTRUM", "#row > 50 && #row <= 100");
      ReportExpected(msg + " succeeded");

      const Table * table = dynamic_cast<const Table *>(ext);
      if (0 == table) ReportUnexpected(msg + ": extension is not a table");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed");
    }

    delete ext;
  }

  void TestFileManager::readTableTest() {
    std::string msg;
    const Table * table = 0;

    // Find test data directory:
    std::string data_dir = getDataDir();

    // Test opening table read-only:
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

  void TestFileManager::fileStatusTest() {
    // Find test data directory:
    std::string data_dir = getDataDir();

    // Test file name.
    std::string file;

    try {
      // First try existing file.
      file = data_dir + "a1.pha";
      if (IFileSvc::instance().fileExists(file))
        ReportExpected(std::string("IFileSvc::fileExists found file ") + file);
      else
        ReportUnexpected(std::string("IFileSvc::fileExists did not find file ") + file);

      // Next try nonexistent file.
      file = data_dir + "non_existent.pha";
      if (IFileSvc::instance().fileExists(file))
        ReportUnexpected(std::string("IFileSvc::fileExists found file ") + file);
      else
        ReportExpected(std::string("IFileSvc::fileExists did not find file ") + file);

      // Test FitsFileManager's ability to classify a FITS file.
      file = data_dir + "a1.pha";
      if (FitsFileManager::isValid(file))
        ReportExpected(std::string("FitsFileManager::isValid correctly recognized FITS file ") + file);
      else
        ReportUnexpected(std::string("FitsFileManager::isValid incorrectly failed to recognize FITS file ") + file);

      // Next try nonexistent file.
      file = data_dir + "non_existent.pha";
      if (FitsFileManager::isValid(file))
        ReportUnexpected(std::string("FitsFileManager::isValid incorrectly recognized file ") + file);
      else
        ReportExpected(std::string("FitsFileManager::isValid correctly failed to recognize file ") + file);

      // Next try Root file.
      file = data_dir + "merit.root";
      if (FitsFileManager::isValid(file))
        ReportUnexpected(std::string("FitsFileManager::isValid incorrectly recognized file ") + file);
      else
        ReportExpected(std::string("FitsFileManager::isValid correctly failed to recognize file ") + file);

      // Test RootTable's ability to classify a Root file.
      file = data_dir + "merit.root";
      if (RootTable::isValid(file))
        ReportExpected(std::string("RootTable::isValid correctly recognized Root file ") + file);
      else
        ReportUnexpected(std::string("RootTable::isValid incorrectly failed to recognize Root file ") + file);

      // Next try nonexistent file.
      file = data_dir + "non_existent.pha";
      if (RootTable::isValid(file))
        ReportUnexpected(std::string("RootTable::isValid incorrectly recognized file ") + file);
      else
        ReportExpected(std::string("RootTable::isValid correctly failed to recognize file ") + file);

      // Next try Fits file.
      file = data_dir + "a1.pha";
      if (RootTable::isValid(file))
        ReportUnexpected(std::string("RootTable::isValid incorrectly recognized file ") + file);
      else
        ReportExpected(std::string("RootTable::isValid correctly failed to recognize file ") + file);

    } catch (const TipException & x) {
      ReportUnexpected("TestFileManager::fileStatusTest caught unexpected exception while testing properties of " + file, x);
    }
  }

  void TestFileManager::updateKeywordsTest() {
    try {
      std::string file_name = "ft1_kwtest.fits";

      IFileSvc & fs = IFileSvc::instance();

      // Create a new fake ft1 file.
      fs.createFile(file_name, getDataDir() + "ft1.tpl");

      // Change the "telescop" key.
      Header::KeyValCont_t kwds;
      kwds.push_back(Header::KeyValPair_t("TELESCOP", "SLOTHROP"));

      // Update all telescop keys, file-wide.
      fs.updateKeywords(file_name, kwds);

      // Verify that this took effect. Get a file summary.
      FileSummary summary;
      fs.getFileSummary(file_name, summary);

      // Iterate over extensions.
      for (FileSummary::iterator itor = summary.begin(); itor != summary.end(); ++itor) {
        std::auto_ptr<const tip::Extension> ext(fs.readExtension(file_name, itor->getExtId()));

        std::string telescop;
        ext->getHeader()["TELESCOP"].get(telescop);
        if (0 != telescop.compare("SLOTHROP")) throw TipException("IFileSvc::updateKeywords failed to update TELESOP");
      }

      ReportExpected("IFileSvc::updateKeywords worked correctly");
    } catch (const TipException & x) {
      ReportUnexpected("TestFileManager::updateKeywordsTest caught unexpected exception", x);
    }
  }

  void TestFileManager::appendImageTest() {
    try {
      std::vector<long> dims(3);
      dims[0] = 512;
      dims[1] = 512;
      dims[2] = 2;

      // Make sure no image is already there.
      remove("created_image.fits");

      // Try to append image when the file does not exist.
      IFileSvc::instance().appendImage("created_image.fits", "TEST_IMAGE", dims);

      // Try to open this new image.
      const Image * image = IFileSvc::instance().readImage("created_image.fits", "TEST_IMAGE");
      delete image; image = 0;

      // Try to create new image when the file does exist. Clobber should not be
      // relevant because the first image exists, so test that setting clobber to false
      // does not cause failure.
      IFileSvc::instance().appendImage("created_image.fits", "TEST_IMAGE2", dims);

      // Try to open this new image to verify basic function.
      image = IFileSvc::instance().readImage("created_image.fits", "TEST_IMAGE2");
      delete image;

    } catch (const TipException & x) {
      ReportUnexpected("TestFileManager::appendImageTest caught unexpected exception", x);
    }
  }

  void TestFileManager::appendTableTest() {
    try {
      // Make sure no table is already there.
      remove("created_table.fits");

      // Try to append table when the file does not exist.
      IFileSvc::instance().appendTable("created_table.fits", "TEST_TABLE");

      // Try to open this new table.
      const Table * table = IFileSvc::instance().readTable("created_table.fits", "TEST_TABLE");
      delete table; table = 0;

      // Try to create new table when the file does exist. Clobber should not be
      // relevant because the first image exists, so test that setting clobber to false
      // does not cause failure.
      IFileSvc::instance().appendTable("created_table.fits", "TEST_TABLE2");

      // Try to open this new table.
      table = IFileSvc::instance().readTable("created_table.fits", "TEST_TABLE2");
      delete table;

    } catch (const TipException & x) {
      ReportUnexpected("TestFileManager::appendTableTest caught unexpected exception", x);
    }
  }

}
