/** \file TestExtensionData.cxx
    \brief Test IExtensionData and its subclasses, as well as (indirectly) FitsExtensionManager
    and RootExtensionManager.
    \author James Peachey, HEASARC
*/

#include <iostream>
#include <sstream>
#include <string>

#include "FitsExtensionData.h"
#include "RootExtensionData.h"
#include "tip/IExtensionData.h"
#include "tip/TipException.h"

#define MAKE_COMPILATION_FAIL (0)

void ReportError(const std::string & context, int & status, const tip::TipException & x = tip::TipException("")) {
  if (0 == status) status = 1;
  std::cerr << "Unexpected error: " << context;
  const char * what = x.what();
  if (0 != what && '\0' != *what) std::cerr << "\n\twhat() == " << what;
  std::cerr << "\n" << std::endl;
}

void ReportBehavior(const std::string & context, const int &, const tip::TipException & x = tip::TipException("")) {
#if 1
  std::cerr << "Expected behavior: " << context;
  const char * what = x.what();
  if (0 != what && '\0' != *what) std::cerr << "\n\twhat() == " << what;
  std::cerr << "\n" << std::endl;
#endif
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
    const_ext->getFieldNumElements(100);
    msg = "success calling getFieldnumElements(100) from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure calling getFieldnumElements(100) from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

  try {
    // Get a table cell from an image.
    // This is only valid for tables.
    const_ext->getCell(100, 0, 0, 1, tmp_d);
    msg = "success reading a table cell from a const";
    ReportError(msg + " " + ext_type + " object", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    msg = "failure reading a table cell from a const";
    ReportBehavior(msg + " " + ext_type + " object", status, x);
  }

#if MAKE_COMPILATION_FAIL
  ReportError("SHOULD NOT HAVE COMPILED! Calling setNumRecords(100) for const image object", status);
  try {
    // Get number of records from the image:
    // This is only valid for tables.
    const_ext->setNumRecords(100);
  } catch(const TipException & x) {
  }

  ReportError("SHOULD NOT HAVE COMPILED! Calling setCell(...) for const image object", status);
  try {
    // Set a table cell in an image.
    // This is only valid for tables.
    const_ext->setCell(100, 0, 0, tmp_d, tmp_d + 1);
  } catch(const TipException & x) {
  }
#endif

}

int TestExtensionData(const std::string & data_dir, int currentStatus) {
  using namespace tip;

  int status = 0;


  // BEGIN Test error cases for FitsExtensionData constructors.
  try {
    // Blank file name, blank extension name:
    FitsExtensionData tmp_data("", "");
    ReportError("success creating FitsExtensionData with blank file name and extension name", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior("failure creating FitsExtensionData with blank file name and extension name", status, x);
  }

  try {
    // Blank file name, non-blank extension name:
    FitsExtensionData tmp_data("", data_dir + "a1.fits");
    ReportError("success creating FitsExtensionData with blank file name and non-blank extension name", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior("failure creating FitsExtensionData with blank file name and non-blank extension name", status, x);
  }

  try {
    // Non-blank file name (doesn't exist), blank extension name:
    FitsExtensionData tmp_data("non-existent-file.fits", "");
    ReportError("success creating FitsExtensionData object with a non-existent file name and blank extension name", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior("failure creating FitsExtensionData object with a non-existent file name and blank extension name", status, x);
  }

  try {
    // Non-blank file name (doesn't exist), non-blank extension name:
    FitsExtensionData tmp_data("non-existent-file.fits", "SPECTRUM");
    ReportError("success creating FitsExtensionData object with a non-existent file name and valid extension name", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior("failure creating FitsExtensionData object with a non-existent file name and valid extension name", status, x);
  }

  try {
    // File exists, but extension doesn't:
    FitsExtensionData tmp_data(data_dir + "a1.pha", "NON_EXISTENT");
    ReportError("success creating FitsExtensionData object with an existent file and non-existent extension name", status);
  } catch(const TipException & x) {
    // This exception should have been thrown.
    ReportBehavior("failure creating FitsExtensionData object with an existent file and non-existent extension name", status, x);
  }
  // END Test error cases for FitsExtensionData constructors.






  // BEGIN Test success cases for FitsExtensionData constructors.
  // This test object will be used in further tests below so create it at this scope:
  IExtensionData * image = 0;

  try {
    // Valid file name, blank extension name:
    image = new FitsExtensionData(data_dir + "a1.pha", "");
    ReportBehavior("success creating FitsExtensionData with valid file name and blank extension name", status);
  } catch(const TipException & x) {
    ReportError("failure creating FitsExtensionData with valid file name and blank extension name", status, x);
  }

  // This test object will be used in further tests below so create it at this scope:
  IExtensionData * table = 0;

  try {
    // Valid file name, blank extension name:
    table = new FitsExtensionData(data_dir + "a1.pha", "SPECTRUM");
    ReportBehavior("success creating FitsExtensionData with valid file name and blank extension name", status);
  } catch(const TipException & x) {
    ReportError("failure creating FitsExtensionData with valid file name and blank extension name", status, x);
  }
  // END Test success cases for FitsExtensionData constructors.






  // BEGIN Test error cases for const FitsExtensionData methods for an image extension.
  // Skip these tests if image object was not successfully opened above:
  if (0 == image) {
    ReportError("image pointer is null; skipping some tests", status);
  } else {
    // Note that image points to the primary HDU, which is an image.
    // Use a constant pointer from here on down:
    const IExtensionData * const_image = image;

    // Test operations which should fail for any/all extensions regardless of whether they are tables or images:
    // The following call generates some errors containing the string "from a const image object"
    TestCommonErrors(const_image, "image", status);

    try {
      // Get number of records from the image:
      // This operation is only valid for tables, so it should fail:
      Index_t tmp_num_rec = const_image->getNumRecords();
      std::ostringstream tmp_msg;
      tmp_msg << "success calling getNumRecords() from a const image object";
      tmp_msg << ", number of records is " << tmp_num_rec;
      ReportError(tmp_msg.str(), status);
    } catch(const TipException & x) {
      // This exception should have been thrown.
      ReportBehavior("failure calling getNumRecords() from a const image object", status, x);
    }

  }
  // END Test error cases for const FitsExtensionData methods.






  // If currentStatus is non-0, keep it. Otherwise return the status from this test.
  status = (0 == currentStatus) ? status : currentStatus;

  return status;
}
