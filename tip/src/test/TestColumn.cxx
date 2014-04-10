/** \file TestColumn.cxx
    \brief Implementation of class to perform detailed testing of column abstractions.
    \author James Peachey, HEASARC
*/
#include <cstdlib>
#include <limits>
#include <list>
#include <memory>
#include <sstream>
#include <iostream>
#include "fitsio.h"

#include "FitsColumn.h"
#include "FitsPrimProps.h"
#include "FitsTable.h"
#include "TestColumn.h"

#include "tip/IFileSvc.h"

#ifndef WIN32
#include <math.h>
#endif

namespace {
  double s_tip_nan() {
    double my_nan = 0.;
    if (std::numeric_limits<double>::has_quiet_NaN) {
      my_nan = std::numeric_limits<double>::quiet_NaN();
    } else {
#if defined(WIN32) && !defined(NAN)
      unsigned long s_lnan[2] = { 0xffffffff, 0x7fffffff };
      my_nan = *(double *) s_lnan;
#elif !defined(WIN32)
      // Seems to be in math.h in Linux and OSX implementations.
      my_nan = nan("NAN");
#else
      // Try this and hope it works. If this is not compiling, it is necessary to determine
      // how to create a quiet NAN value on this platform.
      my_nan = NAN;
#endif
    }
    return my_nan;
  }
  static double s_dnan(s_tip_nan());
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

    // Write and read back a maximum 32-bit binary value to test read/write of CFITSIO 32X type
        try {
          FitsTable table("a1-copy.pha", "SPECTRUM", "", false);
          // Create a new field to hold the test data
          table.appendField("TBIT_COL", "32X");

          BitStruct writeVal = 0x7F3F1F0F;
          table.getColumn(table.getFieldIndex("TBIT_COL"))->set(0, writeVal);

          BitStruct readVal;
          table.getColumn(table.getFieldIndex("TBIT_COL"))->get(0, readVal);

          // Confirm that the data read is equivalent to the data written
          if (readVal.m_bit != writeVal.m_bit) {
            ReportUnexpected("TestColumn::test() did not read 0x7F3F1F0F!");
          } else {
		  ReportExpected("TestColumn::test() wrote/read equivalent 32X values.");
          }
	
          std::cout << "One more time, just for kicks! \n" << std::endl;

          // Read/Write to a new row to confirm everything is A-OK.
          writeVal = 0x0F1F3F7F;
          table.getColumn(table.getFieldIndex("TBIT_COL"))->set(1, writeVal);
	  
          table.getColumn(table.getFieldIndex("TBIT_COL"))->get(1, readVal);

          if (readVal.m_bit != writeVal.m_bit) {
            ReportUnexpected("TestColumn::test() did not read 0x0F1F3F7F!");
          } else {
            ReportExpected("TestColumn::test() wrote/read equivalent 32X values, for the second time.");
          }

        } catch (const TipException & x) {
	  ReportUnexpected("TestColumn::test() failed to read/write equivalent values!", x);
        }

    // Check for valid behavior for boolean values with leading/trailing blanks, scalar and vector behavior.
    try {
      // Set file names.
      std::string out_file("testFitsColumnByMH.fits");
      std::string tpl_file("");
      std::string table_name("TEST_DATA");

      // Create a FITS file for following tests.
      IFileSvc & file_svc(tip::IFileSvc::instance());
      file_svc.createFile(out_file, tpl_file);
      file_svc.appendTable(out_file, table_name);
      std::auto_ptr<Table> table(file_svc.editTable(out_file, table_name));

      // Add columns to the FITS file.
      table->appendField("1L_COLUMN", "1L");
      table->appendField("1I_COLUMN", "1I");
      table->appendField("3L_COLUMN", "3L");
      table->appendField("3I_COLUMN", "3I");
      table->appendField("PI_COLUMN", "PI");
      table->appendField("TBIT_COL", "32X");

      // Add one row to the FITS table.
      table->setNumRecords(1);
      Table::Record record(table.get(), 0);

      // Set integer numbers to 1I_COLUMN and 3I_COLUMN columns.
      record["1I_COLUMN"].set(7);
      std::vector<int> vector_int_initial(3);
      vector_int_initial[0] = 1;
      vector_int_initial[1] = 2;
      vector_int_initial[2] = 3;
      record["3I_COLUMN"].set(vector_int_initial);

      // Set Boolean values to 1L_COLUMN and 3L_COLUMN column.
      record["1L_COLUMN"].set(false);
      std::vector<bool> vector_bool_expected(3);
      vector_bool_expected[0] = false;
      vector_bool_expected[1] = true;
      vector_bool_expected[2] = false;
      record["3L_COLUMN"].set(vector_bool_expected);

      // Set three integer numbers to PI_COLUMN column.
      std::vector<int> vector_int_expected(5);
      vector_int_expected[0] = 11;
      vector_int_expected[1] = 22;
      vector_int_expected[2] = 33;
      vector_int_expected[3] = 44;
      vector_int_expected[4] = 55;
      record["PI_COLUMN"].set(vector_int_expected);

      // Case 1: Read out a variable-length array using a vector of string.
      // --- With FitsColumn.cxx r1.24, tip::Table::Record always returns an array with a single element.
      std::vector<std::string> vector_string_result;
      record["PI_COLUMN"].get(vector_string_result);
      if (vector_string_result.size() != vector_int_expected.size()) {
        std::ostringstream os;
        os << "tip::Table::Record::get(std::vector<std::string>) returned a string array with " <<
          vector_string_result.size() << " elements, not " << vector_int_expected.size() << " as expected.";
        ReportUnexpected(os.str());
      }

      // Case 2: Set a numerical value as a string with extra leading or trailing spaces.
      // --- With FitsColumn.cxx r1.24, this makes tip::Table::Record throw an exception.
      std::list<std::string> test_string_list;
      test_string_list.push_back(" ");
      test_string_list.push_back("123");
      test_string_list.push_back("123   ");
      test_string_list.push_back("   123");
      test_string_list.push_back("   123   ");
      for (std::list<std::string>::const_iterator itor = test_string_list.begin(); itor != test_string_list.end(); ++itor) {
        const std::string & test_string(*itor);
        try {
          record["1I_COLUMN"].set(test_string);
        } catch (...) {
          std::ostringstream os;
          os << "tip::Table::Record::set(\"" << test_string << "\") unexpectedly threw an exception.";
          ReportUnexpected(os.str());
        }
      }

      // Case 3: Set a vector of numerical values as a vector of strings with extra leading or trailing spaces.
      // --- With FitsColumn.cxx r1.24, this makes tip::Table::Record throw an exception.
      std::vector<std::string> vector_string_input(3);
      vector_string_input[0] = "111";
      vector_string_input[1] = "222";
      vector_string_input[2] = "333";
      for (std::list<std::string>::const_iterator itor = test_string_list.begin(); itor != test_string_list.end(); ++itor) {
        vector_string_input[1] = *itor;
        try {
          record["3I_COLUMN"].set(vector_string_input);
        } catch (...) {
          std::ostringstream os;
          os << "tip::Table::Record::set((\"" << vector_string_input[0] << "\", \"" <<
            vector_string_input[1] << "\", \"" << vector_string_input[2] << "\")) unexpectedly threw an exception.";
          ReportUnexpected(os.str());
        }
      }

      // Case 4: Set a Boolean value as a string with extra leading or trailing spaces.
      // --- With FitsColumn.cxx r1.24, this makes tip::Table::Record throw an exception.
      test_string_list.clear();
      test_string_list.push_back("T");
      test_string_list.push_back("T   ");
      test_string_list.push_back("   T");
      test_string_list.push_back("   T   ");
      for (std::list<std::string>::const_iterator itor = test_string_list.begin(); itor != test_string_list.end(); ++itor) {
        const std::string & test_string(*itor);
        try {
          record["1L_COLUMN"].set(test_string);
        } catch (...) {
          std::ostringstream os;
          os << "tip::Table::Record::set(\"" << test_string << "\") unexpectedly threw an exception.";
          ReportUnexpected(os.str());
        }
      }

      // Case 5: Set a vector of Boolean values as a vector of strings with extra leading or trailing spaces.
      // --- With FitsColumn.cxx r1.24, this makes tip::Table::Record throw an exception.
      // NOTE: This test case causes a FITSIO error (CFITSIO ERROR 312: bad binary table datatype),
      //       the underlying FITSIO function (fits_write_col_null, which then calls ffpclb for this case)
      //       does not support TLOGICAL as of this writing (CFITSIO version 3.26). So, this test is
      //       disabled for now. This test case should be revived unchanged and the test result should be checked.
    #if 0
      vector_string_input[0] = "F";
      vector_string_input[1] = "F";
      vector_string_input[2] = "F";
    #endif
    #if 1
      // Instead, run the following test cases with white spaces only to see they are handled correctly,
      // which does not reveal any bug in tip::FitsColumn class in FitsColumn.cxx r1.24.
      vector_string_input[0] = "";
      vector_string_input[1] = "";
      vector_string_input[2] = "";
      test_string_list.clear();
      test_string_list.push_back(" ");
    #endif
      for (std::list<std::string>::const_iterator itor = test_string_list.begin(); itor != test_string_list.end(); ++itor) {
        vector_string_input[1] = *itor;
        try {
          record["3L_COLUMN"].set(vector_string_input);
        } catch (...) {
          std::ostringstream os;
          os << "tip::Table::Record::set((\"" << vector_string_input[0] << "\", \"" <<
            vector_string_input[1] << "\", \"" << vector_string_input[2] << "\")) unexpectedly threw an exception.";
          ReportUnexpected(os.str());
        }
      }

      // Case 4: Read out the vector of bool.
      // --- With FitsColumn.cxx r1.24, This causes a segmentation fault.
      std::vector<bool> vector_bool_result;
      record["3L_COLUMN"].get(vector_bool_result);

      // Return exit status.
    } catch (const TipException & x) {
      ReportUnexpected("TestColumn::test() caught unexpected exception while testing leading and trailing blanks in bools", x);
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
