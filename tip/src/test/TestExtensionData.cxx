/** \file TestExtensionData.cxx
    \brief Test FitsImage, FitsTable and RootTable.
    \author James Peachey, HEASARC
*/

#include <iostream>
#include <sstream>
#include <string>

// The following are needed for chmod:
#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "FitsImage.h"
#include "FitsTable.h"
#include "RootTable.h"
#include "TestExtensionData.h"
#include "tip/Image.h"
#include "tip/KeyRecord.h"
#include "tip/Table.h"
#include "tip/TipException.h"

#define MAKE_COMPILATION_FAIL (0)

void ReportBehavior(const std::string & context, const int &, const tip::TipException & x = tip::TipException("")) {
#if 1
  std::cerr << "Expected behavior: " << context;
  const char * what = x.what();
  if (0 != what && '\0' != *what) std::cerr << "\n\twhat() == " << what;
  std::cerr << "\n" << std::endl;
#endif
}

void ReportError(const std::string & context, int & status, const tip::TipException & x = tip::TipException("")) {
  if (0 == status) status = 1;
  std::cerr << "Unexpected behavior: " << context;
  const char * what = x.what();
  if (0 != what && '\0' != *what) std::cerr << "\n\twhat() == " << what;
  std::cerr << "\n" << std::endl;
}

void ReportWarning(const std::string & msg) {
  std::cerr << "WARNING: " << msg << std::endl;
}

template <typename T>
std::string ToString(const T & value) {
  std::ostringstream os;
  os << value;
  return os.str();
}

template <typename ExtData>
void TestConstructorErrors(const std::string & class_name, const std::string & file_name, int & status) {
  using namespace tip;
  std::string msg;
  try {
    // Blank file name, blank extension name:
    msg = "with blank file name and blank extension name";
    ExtData tmp_data("", "");
    ReportError(std::string("success creating ") + class_name + " " + msg, status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior(std::string("failure creating ") + class_name + " " + msg, status, x);
  }

  try {
    // Blank file name, non-blank extension name:
    msg = "with blank file name and non-blank extension name";
    ExtData tmp_data("", file_name);
    ReportError(std::string("success creating ") + class_name + " " + msg, status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior(std::string("failure creating ") + class_name + " " + msg, status, x);
  }

  try {
    // Non-blank file name (doesn't exist), blank extension name:
    msg = "with a non-existent file name and blank extension name";
    ExtData tmp_data("non-existent-file.fits", "");
    ReportError(std::string("success creating ") + class_name + " " + msg, status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior(std::string("failure creating ") + class_name + " " + msg, status, x);
  }

  try {
    // Non-blank file name (doesn't exist), non-blank extension name:
    msg = "with a non-existent file name and valid extension name";
    ExtData tmp_data("non-existent-file.fits", "SPECTRUM");
    ReportError(std::string("success creating ") + class_name + " " + msg, status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior(std::string("failure creating ") + class_name + " " + msg, status, x);
  }

  try {
    // File exists, but extension doesn't:
    msg = "with an existent file and non-existent extension name";
    ExtData tmp_data(file_name, "NON_EXISTENT");
    ReportError(std::string("success creating ") + class_name + " " + msg, status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior(std::string("failure creating ") + class_name + " " + msg, status, x);
  }
}

// Perform operations on a valid const Table object which are expected to fail.
void TestCommonErrors(const tip::Table * const_ext, const std::string & ext_type, int & status) {
  using namespace tip;
  std::string msg;
  double tmp_d;

  try {
    // Get an unnamed keyword.
    const_ext->getHeader().getKeyword("", tmp_d);
    msg = "success reading unnamed keyword from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading unnamed keyword from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get a non-existent keyword.
    const_ext->getHeader().getKeyword("fake_kwd", tmp_d);
    msg = "success reading non-existent keyword from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading non-existent keyword from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get index of a field from the image:
    // This is only valid for tables.
    const_ext->getFieldIndex("fake_fld");
    msg = "success calling getFieldIndex(\"fake_fld\") from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure calling getFieldIndex(\"fake_fld\") from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get number of elements in a field from the image:
    // This is only valid for tables.
    const_ext->getColumn(-1)->getNumElements();
    msg = "success calling getNumElements(-1) from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure calling getNumElements(-1) from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get a table cell from an image.
    // This is only valid for tables.
    const_ext->getColumn(-1)->get(0, tmp_d);
    msg = "success reading a table cell from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading a table cell from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

#if MAKE_COMPILATION_FAIL
  throw TipException("SHOULD NOT HAVE COMPILED! Calling setNumRecords(-1) for const image object");
  try {
    // Get number of records from the image:
    // This is only valid for tables.
    const_ext->setNumRecords(-1);
  } catch(const TipException & x) {
  }

  throw TipException("SHOULD NOT HAVE COMPILED! Calling set(...) for const image object");
  try {
    // Set a table cell in an image.
    // This is only valid for tables.
    const_ext->getColumn(-1)->set(0, tmp_d);
  } catch(const TipException & x) {
  }
#endif

}

// Perform operations on a valid const Image object which are expected to fail.
//void TestCommonErrors(const tip::Image * const_ext, const std::string & ext_type, int & status) {
void TestCommonErrors(const tip::Image * , const std::string & , int & ) {
#if 0
  using namespace tip;
  std::string msg;
  double tmp_d;

  try {
    // Get an unnamed keyword.
    const_ext->getHeader().getKeyword("", tmp_d);
    msg = "success reading unnamed keyword from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading unnamed keyword from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get a non-existent keyword.
    const_ext->getHeader().getKeyword("fake_kwd", tmp_d);
    msg = "success reading non-existent keyword from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading non-existent keyword from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get index of a field from the image:
    // This is only valid for tables.
    const_ext->getFieldIndex("fake_fld");
    msg = "success calling getFieldIndex(\"fake_fld\") from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure calling getFieldIndex(\"fake_fld\") from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get number of elements in a field from the image:
    // This is only valid for tables.
    const_ext->getColumn(-1)->getNumElements();
    msg = "success calling getNumElements(-1) from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure calling getNumElements(-1) from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get a table cell from an image.
    // This is only valid for tables.
    const_ext->getColumn(-1)->get(0, tmp_d);
    msg = "success reading a table cell from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading a table cell from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

#if MAKE_COMPILATION_FAIL
  throw TipException("SHOULD NOT HAVE COMPILED! Calling setNumRecords(-1) for const image object");
  try {
    // Get number of records from the image:
    // This is only valid for tables.
    const_ext->setNumRecords(-1);
  } catch(const TipException & x) {
  }

  throw TipException("SHOULD NOT HAVE COMPILED! Calling set(...) for const image object");
  try {
    // Set a table cell in an image.
    // This is only valid for tables.
    const_ext->getColumn(-1)->set(0, tmp_d);
  } catch(const TipException & x) {
  }
#endif
#endif

}

// Test reading an entire column from an extension, one row at a time:
void TestReadField(const tip::Table * const_ext, const std::string & field_name, const std::string & ext_type, int & status) {
  using namespace tip;
  std::string msg;
  try {
    // First, get the position of the field in the table:
    msg = std::string("getFieldIndex(\"") + field_name + "\")";
    Index_t field_index = const_ext->getFieldIndex(field_name);
    ReportBehavior(msg + " succeeded for const " + ext_type + " object", status);

    try {
      // Next, get the number of records in the table:
      msg = "getNumRecords()";
      Index_t num_rec = const_ext->getNumRecords();
      ReportBehavior(msg + " succeeded for const " + ext_type + " object", status);

      try {
        // Next, get the number of elements in each cell for this field:
        msg = std::string("getNumElements(\"") + ToString(field_index) + "\")";
        Index_t num_elements = const_ext->getColumn(field_index)->getNumElements();
        ReportBehavior(msg + " succeeded for const " + ext_type + " object", status);

        if (0 >= num_elements) {
          // Something's wrong, so don't try to allocate an array with non-positive number of elements!
          msg += " returned a non-positive number of elements";
          ReportError(msg + " from a const " + ext_type + " object", status);
        } else {
          const IColumn * column = const_ext->getColumn(field_index);
          if (1 == num_elements && !column->isScalar())
            ReportError("number of elements in column is 1, but IColumn::isScalar() returned false", status);
          else if (1 != num_elements && column->isScalar())
            ReportError("number of elements in column is not 1, but IColumn::isScalar() returned true", status);
          else if (1 == num_elements && column->isScalar())
            ReportBehavior("number of elements is 1, and IColumn::isScalar() returned true", status);
          else if (1 != num_elements && !column->isScalar())
            ReportBehavior("number of elements is not 1, and IColumn::isScalar() returned false", status);

          // Allocate an array to hold the values read from a single cell of this field:
          std::vector<double> tmp_dv(num_elements);
          double tmp_d;
          try {
            // Iterate over all records, reading them:
            for (Index_t ii = 0; ii < num_rec; ++ii) {
              if (column->isScalar()) {
                msg = std::string("getColumn(") + ToString(field_index) + ")->get(" + ToString(ii) + ", tmp_d)";
                column->get(ii, tmp_d);
              } else {
                msg = std::string("getColumn(") + ToString(field_index) + ")->get(" + ToString(ii) + ", tmp_dv)";
                column->get(ii, tmp_dv);
              }
            }
            msg = std::string("getColumn(") + ToString(field_index) + ")->get(ii, tmp_dv)";
            ReportBehavior(msg + " succeeded for all " + ToString(num_rec) + " records in const " + ext_type + " object", status);

          } catch(const TipException & x) {
            ReportError(msg + " failed for const " + ext_type + " object", status, x);
          }
        }

      } catch(const TipException & x) {
        ReportError(msg + " failed for const " + ext_type + " object", status, x);
      }

    } catch(const TipException & x) {
      ReportError(msg + " failed for const " + ext_type + " object", status, x);
    }

  } catch(const TipException & x) {
    ReportError(msg + " failed for const " + ext_type + " object", status, x);
  }
}

int TestExtensionData(const std::string & data_dir, int currentStatus) {
  using namespace tip;

  int status = 0;

  // Name of a message string, used in reporting errors:
  std::string msg;

  // Test error cases for FitsTable constructors:
  TestConstructorErrors<FitsTable>("FitsTable", data_dir + "a1.pha", status);

  // BEGIN Test success cases for FitsTable, FitsImage constructors.
  // This test object will be used in further tests below so create it at this scope:
  Image * image = 0;

  try {
    // Valid file name, valid extension name:
    image = new FitsImage(data_dir + "a1.pha", "", "", false);
    ReportBehavior("success creating FitsImage with valid file name and valid extension name", status);
  } catch(const TipException & x) {
    ReportError("failure creating FitsImage with valid file name and valid extension name", status, x);
  }

  // This test object will be used in further tests below so create it at this scope:
  Table * table = 0;

  try {
    // Valid file name, valid extension name:
    table = new FitsTable(data_dir + "a1.pha", "SPECTRUM", "", false);
    ReportBehavior("success creating FitsTable with valid file name and valid extension name", status);
  } catch(const TipException & x) {
    ReportError("failure creating FitsTable with valid file name and valid extension name", status, x);
  }
  // END Test success cases for FitsTable, FitsImage constructors.






  // BEGIN Test const FitsImage methods for an image extension.
  // Skip these tests if image object was not successfully opened above:
  if (0 == image) {
    ReportError("image pointer is null; skipping some tests", status);
  } else {
    // Note that image points to the primary HDU, which is an image.
    // Use a constant pointer from here on down:
    const Image * const_ext = image;

    // Test operations which should fail for any/all extensions regardless of whether they are tables or images:
    // The following call generates some errors containing the string "from a const image object"
    TestCommonErrors(const_ext, "image", status);

  }
  // END Test const FitsImage methods for an image extension.





  // BEGIN Test const FitsImage methods for a table extension.
  // Skip these tests if table object was not successfully opened above:
  if (0 == table) {
    ReportError("table pointer is null; skipping some tests", status);
  } else {
    // Name of the extension type, used in reporting errors:
    std::string ext_type = "table";

    // Note that table points to the primary HDU, which is an table.
    // Use a constant pointer from here on down:
    const Table * const_ext = table;

    // Test operations which should fail for any/all extensions regardless of whether they are tables or tables:
    // The following call generates some errors containing the string "from a const table object"
    TestCommonErrors(const_ext, ext_type, status);

    // Test table operations which should succeed.
    try {
      // Dummy variable for holding double values obtained from the table.
      double tmp_d;

      // Get a valid double keyword, and confirm its value.
      const_ext->getHeader().getKeyword("src_thet", tmp_d);
      if (-999. == tmp_d) {
        msg = "success calling getKeyword(\"src_thet\") from a const";
        ReportBehavior(msg + " " + ext_type + " object", status);
      } else {
        std::ostringstream os;
        os << "getKeyword(\"src_thet\") returned " << tmp_d << " not 999. from a const";
        ReportError(os.str() + " " + ext_type + " object", status);
      }

      // Get valid keyword comment, and confirm its value.
      std::string expected_s = "Theta to rad src [deg]";
      std::string tmp_s = const_ext->getHeader()["src_thet"].getComment();
      if (tmp_s == expected_s) {
        msg = "success calling Keyword::getComment from a const";
        ReportBehavior(msg + " " + ext_type + " object", status);
      } else {
        msg = "Keyword::getComment returned \"" + tmp_s + "\" not \"" + expected_s + "\" when called for a const";
        ReportError(msg + " " + ext_type + " object", status);
      }

      // Get valid keyword units. Despite the [deg] in the comment, reading unit will return a blank string,
      // because units must be at the beginning of the comment.
      expected_s = "";
      tmp_s = const_ext->getHeader()["src_thet"].getUnit();
      if (tmp_s == expected_s) {
        msg = "success calling Keyword::getUnit from a const";
        ReportBehavior(msg + " " + ext_type + " object", status);
      } else {
        msg = "Keyword::getUnit returned \"" + tmp_s + "\" not \"" + expected_s + "\" when called for a const";
        ReportError(msg + " " + ext_type + " object", status);
      }

      // Set keyword unit, which should in effect prepend [deg] to the comment.
      table->getHeader()["src_thet"].setUnit("deg");
      
      // Check that the keyword unit was modified, by reading the comment again and seeing if it changed.
      expected_s = "[deg] Theta to rad src [deg]";
      tmp_s = const_ext->getHeader()["src_thet"].getComment();
      if (tmp_s == expected_s) {
        msg = "success calling Keyword::setUnit for a";
        ReportBehavior(msg + " " + ext_type + " object", status);
      } else {
        msg = "when verifying setUnit, Keyword::getComment returned \"" + tmp_s + "\" not \"" + expected_s +
          "\" when called for a const";
        ReportError(msg + " " + ext_type + " object", status);
      }

      // Also check whether getUnit works by getting the units again. Now they should be deg.
      expected_s = "deg";
      tmp_s = const_ext->getHeader()["src_thet"].getUnit();
      if (tmp_s == expected_s) {
        msg = "success calling Keyword::getUnit from a const";
        ReportBehavior(msg + " " + ext_type + " object", status);
      } else {
        msg = "Keyword::getUnit returned \"" + tmp_s + "\" not \"" + expected_s + "\" when called for a const";
        ReportError(msg + " " + ext_type + " object", status);
      }

      // Reset keyword comment in order to undo the change above.
      expected_s = "Theta to rad src [deg]";
      table->getHeader()["src_thet"].setComment(expected_s);
      
      // Check that the comment was modified, by reading the comment and seeing if it is different.
      tmp_s = const_ext->getHeader()["src_thet"].getComment();
      if (tmp_s == expected_s) {
        msg = "success calling Keyword::setComment for a";
        ReportBehavior(msg + " " + ext_type + " object", status);
      } else {
        msg = "when verifying setUnit, Keyword::getComment returned \"" + tmp_s + "\" not \"" + expected_s +
          "\" when called for a const";
        ReportError(msg + " " + ext_type + " object", status);
      }

    } catch(const TipException & x) {
      msg = "failure calling getKeyword(\"src_thet\") from a const";
      ReportError(msg + " " + ext_type + " object", status, x);
    }

    // Read an entire column, which will involve calling all important functions:
    TestReadField(const_ext, "channel", "table", status);
  }
  // END Test const FitsTable methods for a table extension.





  // Test error cases for RootTable constructors:
  TestConstructorErrors<RootTable>("RootTable", data_dir + "merit.root", status);

  // BEGIN Test success cases for RootTable constructors.
  // This test object will be used in further tests below so create it at this scope:
  delete table; table = 0;

  try {
    // Valid file name, valid extension name:
    table = new RootTable(data_dir + "merit.root", "1");
    ReportBehavior("success creating RootTable with valid file name and valid extension name", status);
  } catch(const TipException & x) {
    ReportError("failure creating RootTable with valid file name and valid extension name", status, x);
  }
  // END Test success cases for RootTable constructors.






  // BEGIN Test const RootTable methods for a table extension.
  // Skip these tests if table object was not successfully opened above:
  if (0 == table) {
    ReportError("table pointer is null; skipping some tests", status);
  } else {
    // Name of the extension type, used in reporting errors:
    std::string ext_type = "table";

    // Note that table points to the primary HDU, which is an table.
    // Use a constant pointer from here on down:
    const Table * const_ext = table;

    // Test operations which should fail for any/all extensions regardless of whether they are tables or tables:
    // The following call generates some errors containing the string "from a const table object"
    TestCommonErrors(const_ext, ext_type, status);

    // Test table operations which should succeed.
    // Note that keyword access is not supported for Root files.

    // Read an entire column, which will involve calling all important functions:
    TestReadField(const_ext, "McEnergy", "table", status);
  }
  // END Test const RootTable methods for a table extension.






  // Clean up:
  delete table; table = 0;
  delete image; image = 0;

  // If currentStatus is non-0, keep it. Otherwise return the status from this test.
  status = (0 == currentStatus) ? status : currentStatus;

  return status;
}

namespace tip {

  TestExtensionData::TestExtensionData(): m_read_only_extension(0), m_writable_extension(0) {}

  TestExtensionData::~TestExtensionData() throw() { delete m_writable_extension; delete m_read_only_extension; }

  int TestExtensionData::test(int status) {
    setStatus(status);

    // Test read-only access:
    testReadOnly();

    // Test read-write access:
    testReadWrite();

    // Test copying:
    testCopy();

    // Test keyword iterator:
    testKeywordItor();

    return getStatus();
  }

  void TestExtensionData::testReadOnly() {
    std::string msg;

    std::string file_name = getDataDir() + "a1.pha";

    // There are two types of read-only access: 1) a file which is write-protected on disk may be opened
    // without explicitly specifying read-only access to the object representing it. 2) A file which is
    // writable on disk may be opened specifically for read-only access. In both cases the object should
    // behave identically.

    // Open read-write a file which is write-protected. This should be possible, but all non-const methods
    // should throw:
#ifndef WIN32
    try {
      // Make sure file is not writable for this test:
      chmod(file_name.c_str(), S_IRUSR | S_IRGRP | S_IROTH);
  
      msg = std::string("attempt to open extension SPECTRUM in write-protected file ") + file_name;
      try {
        m_read_only_extension = new FitsTable(file_name, "SPECTRUM", "", false);
        ReportExpected(msg + " succeeded");
      } catch(const TipException & x) {
        ReportUnexpected(msg + " failed", x);
        ReportWarning("tests for proper read-only access to write-protected file will be skipped!");
      }
  
      confirmReadOnly(m_read_only_extension);
      delete m_read_only_extension; m_read_only_extension = 0;
    } catch (...) {
      // Make sure file is writable after this test, even if the test failed in some unexpected way:
      chmod(file_name.c_str(), S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
      throw;
    }

    // Make sure file is writable after this test:
    chmod(file_name.c_str(), S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
#endif

    // Open read-only a file which is not write-protected:
    msg = std::string("attempt to open read-only extension SPECTRUM in a writable file ") + file_name;
    try {
      // With no filter and no read_only flag, read-only access will be the outcome, even if the file is writable.
      m_read_only_extension = new FitsTable(file_name, "SPECTRUM", "", true);
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
      ReportWarning("tests for proper read-only access to will be skipped!");
    }

    confirmReadOnly(m_read_only_extension);
  }

  void TestExtensionData::confirmReadOnly(Table * extension) {
    if (0 != extension) {
      std::string msg;

      // Now we have a non-const extension object whose underlying physical file is const (not writable).
      // Any non-const method we call should fail at this point:
      msg = "attempt to write keyword in a non-const object whose file cannot be written to";
      try {
        extension->getHeader().setKeyword("telescop", "GLAST");
        ReportUnexpected(msg + " succeeded");
      } catch(const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      msg = "attempt to resize a non-const table object whose file cannot be written to";
      try {
        extension->setNumRecords(1000);
        ReportUnexpected(msg + " succeeded");
      } catch(const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      msg = "attempt to write a value in a cell of a non-const table object whose file cannot be written to";
      try {
        double tmp_d = 137.;
        extension->getColumn(0)->set(0, tmp_d);
        ReportUnexpected(msg + " succeeded");
      } catch(const TipException & x) {
        ReportExpected(msg + " failed", x);
      }
    }
  }

  void TestExtensionData::testReadWrite() {
    std::string msg;

    std::string file_name = getDataDir() + "a1.pha";

    // Open read-write a file with fixed width field to test correct function of setNumElements.
    file_name = getDataDir() + "a1.pha";
    msg = std::string("attempt to open writable extension SPECTRUM in file ") + file_name;
    try {
      m_writable_extension = new FitsTable(file_name, "SPECTRUM", "#row > 0");
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

    msg = "attempt to confirm vector field is not considered a scalar";
    try {
      if (m_writable_extension->getColumn(1)->isScalar())
        ReportUnexpected(msg + " failed");
      else
        ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

    msg = "attempt to change number of elements in a fixed width field";
    try {
      m_writable_extension->getColumn(1)->setNumElements(1);
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

    msg = "attempt to confirm change to number of elements in a field";
    try {
      if (1 != m_writable_extension->getColumn(1)->getNumElements()) {
        ReportUnexpected(msg + " reported " + toString(m_writable_extension->getColumn(1)->getNumElements()) +
          " elements, not 1");
      } else {
        ReportExpected(msg + " succeeded");
      }
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

    msg = "attempt to confirm that a field which used to be a vector is now a scalar";
    try {
      if (m_writable_extension->getColumn(1)->isScalar())
        ReportExpected(msg + " succeeded");
      else
        ReportUnexpected(msg + " failed");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }
  }

  void TestExtensionData::testCopy() {
    const Table * input = 0;
    Table * output = 0;
    try {
      bool failed = false;

      // Copy cells from a source extension to an output extension.
      // Open input and create copy of input.
      input = new FitsTable(getDataDir() + "a1.pha", "SPECTRUM", "", true);
      output = new FitsTable(getDataDir() + "a1.pha", "SPECTRUM", "#row > 0", false);
  
      // Get the number of records.
      Index_t num_rec = output->getNumRecords();
      if (0 >= num_rec) ReportUnexpected("TestExtensionData::testCopy could not get valid input");
  
      // Invert the column in the copy of the input.
      for (Index_t ii = 0; ii < num_rec; ++ii) {
        output->copyCell(input, 0, ii, 0, num_rec - ii - 1);
        output->copyCell(input, 1, ii, 1, num_rec - ii - 1);
      }
  
      // Confirm success.
      for (Index_t ii = 0; ii < num_rec; ++ii) {
        double src_chan;
        double dest_chan;
        std::vector<double> src_counts(4096);
        std::vector<double> dest_counts(4096);

        // Check channel.
        input->getColumn(0)->get(num_rec - ii - 1, src_chan);
        output->getColumn(0)->get(ii, dest_chan);
        if (src_chan != dest_chan) {
          ReportUnexpected("TestExtensionData::testCopy: one or more scalar values do not match after copying Cells");
          failed = true;
        }

        // Check counts.
        input->getColumn(1)->get(num_rec - ii - 1, src_counts);
        output->getColumn(1)->get(ii, dest_counts);
        for (int jj = 0; jj < 4096; ++jj) {
          if (src_counts[jj] != dest_counts[jj]) {
            ReportUnexpected("TestExtensionData::testCopy: one or more vector values do not match after copying Cells");
            break;
          }
        }
        if (failed) break;
      }

      if (!failed)
        ReportExpected("TestExtensionData::testCopy: using copyCell to copy cells from input to output ITabularData succeeded.");

    } catch(const TipException & x) {
      delete output;
      delete input;
      ReportUnexpected("TestExtensionData::testCopy failed", x);
    }
    delete output;
    delete input;
  }

  void TestExtensionData::testKeywordItor() {
    const Header & header(m_read_only_extension->getHeader());

    bool discrepancy = false;

    // See if the header contains the expected number of keywords.
    Header::KeySeq_t::size_type num_keys = header.end() - header.begin();
    if (142 != num_keys) {
      discrepancy = true;
      std::ostringstream os;
      os << "TestExtensionData::testKeywordItor found " << num_keys << " keywords, not 142 as expected.";
      ReportUnexpected(os.str());
    }
   
    // Make sure getting keywords using the associative container interface yields the same results as
    // the sequential iterator.
    for (Header::ConstIterator itor = header.begin(); itor != header.end(); ++itor) {
      std::string key_name = itor->getName();
      if (!key_name.empty()) {
        std::string assoc_value;
        header[key_name].get(assoc_value);
        if (assoc_value != itor->getValue()) {
          discrepancy = true;
          ReportUnexpected("TestExtensionData::testKeywordItor obtained keyword \"" + key_name + "\" = \"" + assoc_value +
            "\" from associative array, but value was \"" + itor->getValue() + "\" from sequential iterator.");
        }
      }
    }
    if (!discrepancy) ReportExpected("TestExtensionData::testKeywordItor successfully tested keyword sequence iterator.");
  }

}
