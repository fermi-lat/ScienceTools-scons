/** \file TestExtensionData.cxx
    \brief Test IExtensionData and its subclasses, as well as (indirectly) FitsExtensionManager and RootExtensionManager
    and RootExtensionManager.
    \author James Peachey, HEASARC
*/

#include <iostream>
#include <sstream>
#include <string>

#include "FitsExtensionData.h"
#include "RootExtensionData.h"
#include "TestExtensionData.h"
#include "tip/IExtensionData.h"
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

// Perform operations on a valid const extension object which are expected to fail
// regardless of the type (table or image).
void TestCommonErrors(const tip::IExtensionData * const_ext, const std::string & ext_type, int & status) {
  using namespace tip;
  std::string msg;
  double tmp_d[1];

  try {
    // Get an unnamed keyword.
    const_ext->getKeyword("", *tmp_d);
    msg = "success reading unnamed keyword from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading unnamed keyword from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get a non-existent keyword.
    const_ext->getKeyword("fake_kwd", *tmp_d);
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
    const_ext->getFieldNumElements(-1);
    msg = "success calling getFieldNumElements(-1) from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure calling getFieldNumElements(-1) from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get a table cell from an image.
    // This is only valid for tables.
    const_ext->getCell(-1, 0, 0, 1, tmp_d);
    msg = "success reading a table cell from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading a table cell from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

#if MAKE_COMPILATION_FAIL
  ReportError("SHOULD NOT HAVE COMPILED! Calling setNumRecords(-1) for const image object", status);
  try {
    // Get number of records from the image:
    // This is only valid for tables.
    const_ext->setNumRecords(-1);
  } catch(const TipException & x) {
  }

  ReportError("SHOULD NOT HAVE COMPILED! Calling setCell(...) for const image object", status);
  try {
    // Set a table cell in an image.
    // This is only valid for tables.
    const_ext->setCell(-1, 0, 0, tmp_d, tmp_d + 1);
  } catch(const TipException & x) {
  }
#endif

}

// Test reading an entire column from an extension, one row at a time:
void TestReadField(const tip::IExtensionData * const_ext, const std::string & field_name, const std::string & ext_type, int & status) {
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
        msg = std::string("getFieldNumElements(\"") + ToString(field_index) + "\")";
        Index_t num_elements = const_ext->getFieldNumElements(field_index);
        ReportBehavior(msg + " succeeded for const " + ext_type + " object", status);

        if (0 >= num_elements) {
          // Something's wrong, so don't try to allocate an array with non-positive number of elements!
          msg += " returned a non-positive number of elements";
          ReportError(msg + " from a const " + ext_type + " object", status);
        } else {
          // Allocate an array to hold the values read from a single cell of this field:
          double * tmp_dv = new double[num_elements];
          try {
            // Iterate over all records, reading them:
            for (Index_t ii = 0; ii < num_rec; ++ii) {
              msg = std::string("getCell(") + ToString(field_index) + ", " + ToString(ii) + ", 0, " + ToString(num_elements) + ", tmp_dv)";
              const_ext->getCell(field_index, ii, 0, num_elements, tmp_dv);
            }
            msg = std::string("getCell(") + ToString(field_index) + ", ii , 0, " + ToString(num_elements) + ", tmp_dv)";
            ReportBehavior(msg + " succeeded for all " + ToString(num_rec) + " records in const " + ext_type + " object", status);

          } catch(const TipException & x) {
            ReportError(msg + " failed for const " + ext_type + " object", status, x);
          }
          delete [] tmp_dv;
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

  // Test error cases for FitsExtensionData constructors:
  TestConstructorErrors<FitsExtensionData>("FitsExtensionData", data_dir + "a1.pha", status);

  // BEGIN Test success cases for FitsExtensionData constructors.
  // This test object will be used in further tests below so create it at this scope:
  IExtensionData * image = 0;

  try {
    // Valid file name, valid extension name:
    image = new FitsExtensionData(data_dir + "a1.pha", "");
    ReportBehavior("success creating FitsExtensionData with valid file name and valid extension name", status);
  } catch(const TipException & x) {
    ReportError("failure creating FitsExtensionData with valid file name and valid extension name", status, x);
  }

  // This test object will be used in further tests below so create it at this scope:
  IExtensionData * table = 0;

  try {
    // Valid file name, valid extension name:
    table = new FitsExtensionData(data_dir + "a1.pha", "SPECTRUM");
    ReportBehavior("success creating FitsExtensionData with valid file name and valid extension name", status);
  } catch(const TipException & x) {
    ReportError("failure creating FitsExtensionData with valid file name and valid extension name", status, x);
  }
  // END Test success cases for FitsExtensionData constructors.






  // BEGIN Test const FitsExtensionData methods for an image extension.
  // Skip these tests if image object was not successfully opened above:
  if (0 == image) {
    ReportError("image pointer is null; skipping some tests", status);
  } else {
    // Note that image points to the primary HDU, which is an image.
    // Use a constant pointer from here on down:
    const IExtensionData * const_ext = image;

    // Test operations which should fail for any/all extensions regardless of whether they are tables or images:
    // The following call generates some errors containing the string "from a const image object"
    TestCommonErrors(const_ext, "image", status);

    // Another test which should fail: table specific methods
    try {
      // Get number of records from the image:
      // This operation is only valid for tables, so it should fail:
      const_ext->getNumRecords();
      ReportError("success calling getNumRecords() from a const image object", status);
    } catch(const TipException & x) {
      // This exception should have been thrown.
      ReportBehavior("failure calling getNumRecords() from a const image object", status, x);
    }

  }
  // END Test const FitsExtensionData methods for an image extension.





  // BEGIN Test const FitsExtensionData methods for a table extension.
  // Skip these tests if table object was not successfully opened above:
  if (0 == table) {
    ReportError("table pointer is null; skipping some tests", status);
  } else {
    // Name of the extension type, used in reporting errors:
    std::string ext_type = "table";

    // Note that table points to the primary HDU, which is an table.
    // Use a constant pointer from here on down:
    const IExtensionData * const_ext = table;

    // Test operations which should fail for any/all extensions regardless of whether they are tables or tables:
    // The following call generates some errors containing the string "from a const table object"
    TestCommonErrors(const_ext, ext_type, status);

    // Test table operations which should succeed.
    try {
      // Dummy variable for holding double values obtained from the table.
      double tmp_d;

      // Get a valid double keyword.
      const_ext->getKeyword("src_thet", tmp_d);
      msg = "success calling getKeyword(\"src_thet\") from a const";
      ReportBehavior(msg + " " + ext_type + " object", status);
    } catch(const TipException & x) {
      msg = "failure calling getKeyword(\"src_thet\") from a const";
      ReportError(msg + " " + ext_type + " object", status, x);
    }

    // Read an entire column, which will involve calling all important functions:
    TestReadField(const_ext, "channel", "table", status);
  }
  // END Test const FitsExtensionData methods for a table extension.





  // Test error cases for RootExtensionData constructors:
  TestConstructorErrors<RootExtensionData>("RootExtensionData", data_dir + "merit.root", status);

  // BEGIN Test success cases for RootExtensionData constructors.
  // This test object will be used in further tests below so create it at this scope:
  delete table; table = 0;

  try {
    // Valid file name, valid extension name:
    table = new RootExtensionData(data_dir + "merit.root", "1");
    ReportBehavior("success creating RootExtensionData with valid file name and valid extension name", status);
  } catch(const TipException & x) {
    ReportError("failure creating RootExtensionData with valid file name and valid extension name", status, x);
  }
  // END Test success cases for RootExtensionData constructors.






  // BEGIN Test const RootExtensionData methods for a table extension.
  // Skip these tests if table object was not successfully opened above:
  if (0 == table) {
    ReportError("table pointer is null; skipping some tests", status);
  } else {
    // Name of the extension type, used in reporting errors:
    std::string ext_type = "table";

    // Note that table points to the primary HDU, which is an table.
    // Use a constant pointer from here on down:
    const IExtensionData * const_ext = table;

    // Test operations which should fail for any/all extensions regardless of whether they are tables or tables:
    // The following call generates some errors containing the string "from a const table object"
    TestCommonErrors(const_ext, ext_type, status);

    // Test table operations which should succeed.
    // Note that keyword access is not supported for Root files.

    // Read an entire column, which will involve calling all important functions:
    TestReadField(const_ext, "McEnergy", "table", status);
  }
  // END Test const RootExtensionData methods for a table extension.






  // Clean up:
  delete table; table = 0;
  delete image; image = 0;

  // If currentStatus is non-0, keep it. Otherwise return the status from this test.
  status = (0 == currentStatus) ? status : currentStatus;

  return status;
}

namespace tip {

  TestExtensionData::TestExtensionData(): m_read_only_extension(0) {}

  TestExtensionData::~TestExtensionData() throw() { delete m_read_only_extension; }

  int TestExtensionData::test(int status) {
    setStatus(status);

    // Test read-only access:
    testReadOnly();
    return getStatus();
  }

  void TestExtensionData::testReadOnly() {
    std::string msg;

    const std::string & data_dir = getDataDir();

    // Attempt to open a read-only file:
    msg = std::string("attempt to open extension SPECTRUM in write-protected file ") + data_dir + "a1_read_only.pha";
    try {
      m_read_only_extension = new FitsExtensionData(data_dir + "a1_read_only.pha", "SPECTRUM");
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed");
    }
  }

}
