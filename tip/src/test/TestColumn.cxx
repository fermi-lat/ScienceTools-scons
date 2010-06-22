/** \file TestColumn.cxx
    \brief Implementation of class to perform detailed testing of column abstractions.
    \author James Peachey, HEASARC
*/
#include <cstdlib>
#include <memory>

#include "fitsio.h"

#include "FitsColumn.h"
#include "FitsPrimProps.h"
#include "FitsTable.h"
#include "TestColumn.h"

#include "tip/IFileSvc.h"

namespace {
    static unsigned long s_lnan[2] = { 0xffffffff, 0x7fffffff };
    static double & s_dnan = *(double *) s_lnan;
}

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
      FitsTable manager("aeff_DC1-copy.fits", "EA_ALL", "", false);

      std::string units = manager.getColumn(0)->getUnits();
      if ("MeV" == units)
        ReportExpected("TestColumn::test(): ENERGY_LO has units of MeV");
      else
        ReportUnexpected("TestColumn::test(): ENERGY_LO has units of " + units + ", not MeV");

      // Test setting/getting nulls with vector-valued columns.
      std::vector<bool> null_value;
      bool any_null = manager.getColumn(0)->getNull(0, null_value);
      if (any_null)
        ReportExpected("TestColumn::test(): first row of EA_ALL::ENERGY_LO has null values at the outset");
      else
        ReportUnexpected("TestColumn::test(): first row of EA_ALL::ENERGY_LO has no null values at the outset");

      if (36 == null_value.size())
        ReportExpected("TestColumn::test(): first row of EA_ALL::ENERGY_LO has 36 elements at the outset");
      else {
        std::ostringstream os;
        os << "TestColumn::test(): first row of EA_ALL::ENERGY_LO has " << null_value.size() <<
          " elements at the outset, not 36 as expected";
        ReportUnexpected(os.str());
      }

      if (null_value[1] && null_value[3]) {
        ReportExpected("Before setting values, null values found in first row of EA_ALL::ENERGY_LO, "
          "elements #1 and #3 (#2 & #4 in FITS/FV)");
      } else {
        if (!null_value[1])
          ReportUnexpected("Before setting values, null value not found in first row of EA_ALL::ENERGY_LO, "
            "element #1 (#2 in FITS/FV)");
        if (!null_value[3])
          ReportUnexpected("Before setting values, null value not found in first row of EA_ALL::ENERGY_LO, "
            "element #3 (#4 in FITS/FV)");
      }

      // Change the value in the column.
      std::vector<double> new_value(5, 137.);
      new_value[1] = s_dnan;
      new_value[3] = s_dnan;
      manager.getColumn(0)->set(0, new_value);

      any_null = manager.getColumn(0)->getNull(0, null_value);
      if (any_null)
        ReportExpected("TestColumn::test(): first row of EA_ALL::ENERGY_LO has null values after being set");
      else
        ReportUnexpected("TestColumn::test(): first row of EA_ALL::ENERGY_LO has no null values after being set");

      if (5 == null_value.size())
        ReportExpected("TestColumn::test(): first row of EA_ALL::ENERGY_LO has 5 elements after being set");
      else {
        std::ostringstream os;
        os << "TestColumn::test(): first row of EA_ALL::ENERGY_LO has " << null_value.size() <<
          " elements after being set, not 5 as expected";
        ReportUnexpected(os.str());
      }

      if (null_value[1] && null_value[3]) {
        ReportExpected("Null values found in first row of EA_ALL::ENERGY_LO, elements #1 and #3 (#2 & #4 in FITS/FV)");
      } else {
        if (!null_value[1])
          ReportUnexpected("Null value not found in first row of EA_ALL::ENERGY_LO, element #1 (#2 in FITS/FV)");
        if (!null_value[3])
          ReportUnexpected("Null value not found in first row of EA_ALL::ENERGY_LO, element #3 (#4 in FITS/FV)");
      }

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

    // Write and read back an empty string, which should be interpreted as a null value.
    try {
      FitsTable manager("a1-copy.pha", "SPECTRUM", "#row>0", false);

      // First read value as a string to make sure it's not null.
      std::string value;
      manager.getColumn(0)->get(1, value);
      bool is_null = manager.getColumn(0)->isNull(1);

      if (is_null || value.empty()) {
        if (is_null) ReportUnexpected("TestColumn::test() isNull method interpreted a non-null value as a null");
        if (value.empty()) ReportUnexpected("TestColumn::test() read a non-null value as a blank string");
      } else {
        // Set the first element in the column to be a null value using blank string.
        std::string null_string = FitsPrimProps<char *>::undefined();
        manager.getColumn(0)->set(1, null_string);

        // Read back the value to make sure it is null.
        manager.getColumn(0)->get(1, value);
        if (value != null_string)
          ReportUnexpected("TestColumn::test() read what should be a null value as the non-blank string \"" + value + "\"");

        // Confirm that the value is interpreted as a null.
        is_null = manager.getColumn(0)->isNull(1);
        if (!is_null) ReportUnexpected("TestColumn::test() isNull method interpreted what should be a null value as not null");
      }
    } catch (const TipException & x) {
      ReportExpected("TestColumn::test() was not able to read/write null value in a double column", x);
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
