/** \file TestColumn.cxx
    \brief Implementation of class to perform detailed testing of column abstractions.
    \author James Peachey, HEASARC
*/
#include <iostream>
#include <sstream>

#include "fitsio.h"

#include "FitsColumn.h"
#include "FitsExtensionManager.h"
#include "TestColumn.h"

#include "tip/IFileSvc.h"

namespace tip {

  TestColumn::~TestColumn() throw() {}

  /** \brief Perform all detailed tests.
  */
  int TestColumn::test(int status) {
    setStatus(status);

    copyDataFile(getDataDir() + "a1.pha", "a1-copy.pha");
    copyDataFile(getDataDir() + "aeff_DC1.fits", "aeff_DC1-copy.fits");

    return getStatus();
  }

  void TestColumn::copyDataFile(const std::string & in_file, const std::string & out_file) {
    int status = 0;

    // Create output file using input file as a template.
    IFileSvc::instance().createFile(out_file, in_file);

    // Get container of extensions in the file(s).
    FileSummary summary;
    IFileSvc::instance().getFileSummary(in_file, summary);

    // Iterate over all extensions.
    for (FileSummary::const_iterator ext_itor = summary.begin(); ext_itor != summary.end(); ++ext_itor) {
      // Open input extension.
      FitsExtensionManager in_manager(in_file, ext_itor->getExtId(), "", true);

      // Skip images for now.
      if (!in_manager.isTable()) continue;

      // Open output extension.
      FitsExtensionManager out_manager(out_file, ext_itor->getExtId(), "", false);
     
      // Get number of fields in this extension.
      long num_fields = in_manager.getValidFields().size();

      // Get number of records in this extension.
      Index_t num_records = in_manager.getNumRecords();

      // Resize the output table.
      out_manager.setNumRecords(num_records);

      // Iterate over all records:
      for (Index_t record_index = 0; record_index != num_records; ++record_index) {
        // For each record, iterate over all fields.
        for (long field_index = 0; field_index < num_fields; ++field_index) {
          const IColumn * in_col = in_manager.getColumn(field_index);
          IColumn * out_col = out_manager.getColumn(field_index);

          // Test copy column. This tests some of the overloaded get and set methods of FitsColumn as well.
          out_col->copy(in_col, record_index, record_index);
        }
      }

      // A little ugliness to handle variable length columns.
      fitsfile * out_fp = out_manager.getFp();

      fits_compress_heap(out_fp, &status);
      if (0 != status) {
        setStatus(status);
        throw std::runtime_error("Unexpected: TestColumn::copyDataFile could not compress heap");
      }

    }
  }
}
