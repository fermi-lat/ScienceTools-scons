/** \file TestColumn.cxx
    \brief Implementation of class to perform detailed testing of column abstractions.
    \author James Peachey, HEASARC
*/
#include <memory>

#include "fitsio.h"

#include "FitsColumn.h"
#include "FitsTable.h"
#include "TestColumn.h"

#include "tip/IFileSvc.h"

namespace tip {

  TestColumn::~TestColumn() throw() {}

  /** \brief Perform all detailed tests.
  */
  int TestColumn::test(int status) {
    setStatus(status);

    // Test copying a file containing scalars and fixed-width vectors.
    copyDataFile(getDataDir() + "a1.pha", "a1-copy.pha");

    // Test copying a file containing variable-width vectors.
    copyDataFile(getDataDir() + "aeff_DC1.fits", "aeff_DC1-copy.fits");

    try {
      FitsTable manager(getDataDir() + "aeff_DC1.fits", "EA_ALL");

      std::string units = manager.getColumn(0)->getUnits();
      if ("MeV" == units)
        ReportExpected("TestColumn::test(): ENERGY_LO has units of MeV");
      else
        ReportUnexpected("TestColumn::test(): ENERGY_LO has units of " + units + ", not MeV");
    } catch (const TipException & x) {
      ReportUnexpected("TestColumn::test() caught unexpected exception while testing FitsColumn::getUnits", x);
    }

    // Write a string containing a number to a numeric column.
    try {
      FitsTable manager("a1-copy.pha", "SPECTRUM", "#row>0", false);
      manager.getColumn(0)->set(0, "123");
      ReportExpected("TestColumn::test() was able to write a numeric string to a double column");
    } catch (const TipException & x) {
      ReportUnexpected("TestColumn::test() was not able to write a numeric string to a double column", x);
    }

    // Write a string which does not contain a number to a numeric column.
    try {
      FitsTable manager("a1-copy.pha", "SPECTRUM", "#row>0", false);
      manager.getColumn(0)->set(0, "not num");
      ReportUnexpected("TestColumn::test() was able to write a non-numeric string to a double column");
    } catch (const TipException & x) {
      ReportExpected("TestColumn::test() was not able to write a non-numeric string to a double column", x);
    }

    // Test getColumnKeyword facility.
    try {
      FitsTable manager(getDataDir() + "aeff_DC1.fits", "EA_ALL");

      std::string units;
      manager.getColumn(0)->getColumnKeyword("TUNIT").get(units);
      if ("MeV" == units)
        ReportExpected("TestColumn::test(): getColumnKeyword(\"TUNIT\") returned MeV");
      else
        ReportUnexpected("TestColumn::test(): getColumnKeyword(\"TUNIT\") returned \"" + units + "\", not MeV");
    } catch (const TipException & x) {
      ReportUnexpected("TestColumn::test() caught unexpected exception while testing FitsColumn::getColumnKeyword", x);
    }

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
      std::auto_ptr<const Extension> in_ext(IFileSvc::instance().readExtension(in_file, ext_itor->getExtId(), ""));

      // Skip images for now.
      if (!in_ext->isTable()) continue;

      // Cast into Table state.
      const Table * in_table = dynamic_cast<const Table *>(in_ext.get());

      // Open output extension.
      std::auto_ptr<Table> out_table(IFileSvc::instance().editTable(out_file, ext_itor->getExtId(), ""));

      // Get number of fields in this extension.
      long num_fields = in_table->getValidFields().size();

      // Get number of records in this extension.
      Index_t num_records = in_table->getNumRecords();

      // Resize the output table.
      out_table->setNumRecords(num_records);

      // Iterate over all records:
      for (Index_t record_index = 0; record_index != num_records; ++record_index) {
        // For each record, iterate over all fields.
        for (long field_index = 0; field_index < num_fields; ++field_index) {
          const IColumn * in_col = in_table->getColumn(field_index);
          IColumn * out_col = out_table->getColumn(field_index);

          // Test copy column. This tests some of the overloaded get and set methods of FitsColumn as well.
          out_col->copy(in_col, record_index, record_index);
        }
      }

      if (FitsTable * fits_table = dynamic_cast<FitsTable *>(out_table.get())) {

        // A little ugliness to handle variable length columns.
        fitsfile * out_fp = fits_table->getFp();

        fits_compress_heap(out_fp, &status);
        if (0 != status) {
          setStatus(status);
          throw std::runtime_error("Unexpected: TestColumn::copyDataFile could not compress heap");
        }
      }

    }
  }
}
