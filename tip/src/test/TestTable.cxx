/** \file TestTable.cxx
    \brief Definition of class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/

#include <string>
#include <vector>

#include "FitsTable.h"
#include "RootTable.h"
#include "TestTable.h"
#include "tip/Table.h"
#include "tip/tip_types.h"

#define MAKE_COMPILATION_FAIL (0)

namespace tip {

  TestTable::TestTable(): m_fits_table(0), m_root_table(0) {}

  TestTable::~TestTable() throw() {}

  int TestTable::test(int status) {
    // Use inherited status to set initial status
    setStatus(status);

    // Test Table's constructors:
    TableTest();

    // Test new browsing capabilities:
    getValidFieldsTest();

    // Test iterator access:
    readWriteFieldTest();

    // Test iterator access to vector columns:
    readWriteVectorFieldTest();

    // Test appending a field to an existing table.
    appendFieldTest();

    // Test copying records from one table to another.
    copyFieldTest();

    // Clean up.
    delete m_root_table; m_root_table = 0;
    delete m_fits_table; m_fits_table = 0;

    return getStatus();
  }

  void TestTable::TableTest() {
    std::string msg;

    // Find test data directory:
    std::string data_dir = getDataDir();

    // Test constructing a FITS table:
    msg = std::string("opening SPECTRUM extension of ") + data_dir + "a1.pha";
    try {
      m_fits_table = getTable();
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
      ReportWarning("FITS table tests will be skipped!");
    }

    // Test constructing a Root table:
    msg = std::string("opening TTree \"1\" extension of ") + data_dir + "merit.root";
    try {
      Table * data = new RootTable(data_dir + "merit.root", "1");
      m_root_table = data;
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
      ReportWarning("Root table tests will be skipped!");
    }
  }

  void TestTable::getValidFieldsTest() {
    std::string msg;
    if (0 != m_fits_table) {
      msg = "getting field container from FITS table";
      try {
        // Get container of field names:
        const Table::FieldCont & fields = m_fits_table->getValidFields();

        int num_fields = 0;
        for (Table::FieldCont::const_iterator it = fields.begin(); it != fields.end(); ++it) {
          // Don't count "new_chan" which is a field added during the test process.
          if (0 != it->compare("new_chan")) ++num_fields;
        }

        // Test file has 2 fields:
        if (2 == num_fields) ReportExpected(msg + " succeeded");
        else ReportUnexpected(msg + " got " + toString(num_fields) + " fields, not 2");

      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
    }
    if (0 != m_root_table) {
      msg = "getting field container from Root table";
      try {
        // Get container of field names:
        const Table::FieldCont & fields = m_root_table->getValidFields();

        int num_fields = 0;
        for (Table::FieldCont::const_iterator it = fields.begin(); it != fields.end(); ++it) {
          ++num_fields;
        }

        // Test file has 224 fields:
        if (224 == num_fields) ReportExpected(msg + " succeeded");
        else ReportUnexpected(msg + " got " + toString(num_fields) + " fields, not 224");

      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
    }
  }

  void TestTable::readWriteFieldTest() {
    // Test FITS field read/write for channel field:
    readWriteFieldTest(m_fits_table, "FITS", "chaNNel");

    // Test Root field read only for McEnergy field; note that this doesn't test whether the
    // values were read correctly.
    std::vector<double> mc_energy;
    try {
      readFieldTest(m_root_table, "McEnergy", mc_energy);
      ReportExpected("reading McEnergy field from Root table succeeded");
    } catch (const TipException & x) {
      ReportUnexpected("reading McEnergy field from Root table failed", x);
    }
  }

  void TestTable::readWriteFieldTest(Table * table, const std::string & format, const std::string & field_name) {
    std::string msg;
    std::vector<double> orig;
    std::vector<double> modified;
    std::vector<double> read_modified;
    bool no_error = true;
    if (0 != table) {
      // Read original values from table into orig array:
      msg = std::string("testing reading ") + format + " table";
      try {
        readFieldTest(table, field_name, orig);
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
        ReportWarning("readWriteFieldTest is skipping the rest of its tests");
        return;
      }

      // Fill modified array, trying to make sure the contents are distinct from the orig array:
      modified.reserve(orig.size());
      srand(1023);
      for (std::vector<double>::iterator itor = orig.begin(); itor != orig.end(); ++itor) {
        double new_val = short(double(rand()) * std::numeric_limits<short>::max() / RAND_MAX);
        modified.push_back(new_val);
      }

      // Write new array to the table:
      msg = std::string("testing writing ") + format + " table";
      try {
        writeFieldTest(table, field_name, modified);
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
        ReportWarning("readWriteFieldTest is skipping some tests");
        no_error = false;
      }

      if (no_error) {
        // Read and check new values from table:
        msg = std::string("testing reading ") + format + " table values which were just written";
        try {
          readFieldTest(table, field_name, read_modified);

          if (modified == read_modified) ReportExpected(msg + " succeeded");
          else {
            ReportUnexpected("discrepancies found between values which were written and then read");
            ReportWarning("TEST DATA FILE MAY HAVE BEEN CORRUPTED!");
          }
        } catch (const TipException & x) {
          ReportUnexpected(msg + " failed", x);
          ReportWarning("readWriteFieldTest is skipping some tests");
          no_error = false;
        }
      }

      // Rewrite orig array to the table:
      msg = std::string("testing restoring ") + format + " table to its original state";
      try {
        writeFieldTest(table, field_name, orig);
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
        ReportWarning("readWriteFieldTest is skipping some tests");
        no_error = false;
      }

      if (no_error) {
        // Read and check whether original values were successfully restored to the table:
        msg = "testing reading restored values";
        try {
          readFieldTest(table, field_name, read_modified);

          if (orig == read_modified) ReportExpected(msg + " succeeded");
          else {
            ReportUnexpected("discrepancies found between original values and those which were restored");
            ReportWarning("TEST DATA FILE MAY HAVE BEEN CORRUPTED!");
          }
        } catch (const TipException & x) {
          ReportUnexpected(msg + " failed", x);
          ReportWarning("TEST DATA FILE MAY HAVE BEEN CORRUPTED!");
        }
      }

    }
  }

  void TestTable::readFieldTest(const Table * table, const std::string & field_name, std::vector<double> & field_values) {
    if (0 != table) {
      std::string msg;

      // Test error cases:
      msg = std::string("getting scalar-valued \"") + field_name + "\" cell into a local vector variable";
      try {
        // Try to read scalar-valued column into a vector.
        double vec[1];
        (*table->begin())[field_name].get(0, 1, vec);
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      // Find out how many records are in the table:
      Index_t num_records = -1;
      try {
        num_records = table->getNumRecords();
      } catch (const TipException & x) {
        // Don't report this exception: not testing getNumRecords here.
      }

      if (0 > num_records) {
        ReportWarning("readFieldTest had trouble preparing to run; test will be skipped!");
      } else if (0 == num_records) {
        ReportWarning("readfieldTest called for a table with no records; test will be skipped!");
      } else {
        // Clear out whatever's in field_values before filling it.
        field_values.clear();

        // Resize the output array so it has enough room:
        field_values.reserve(num_records);

        // Loop over table, copying all values into output array:
        for (Table::ConstIterator itor = table->begin(); itor != table->end(); ++itor) {
          field_values.push_back((*itor)[field_name].get());

#if MAKE_COMPILATION_FAIL
          throw TipException("SHOULD NOT HAVE COMPILED! Assigning to a const Table::Cell object");
          (*itor)[field_name].set(1.);
#endif

        }
      }
    }
  }

  void TestTable::writeFieldTest(Table * table, const std::string & field_name, const std::vector<double> & field_values) {
    if (0 != table) {
      std::string msg;

      // Test error cases:
      msg = std::string("writing scalar-valued \"") + field_name + "\" cell from a local vector variable";
      try {
        // Try to write scalar-valued column into a vector.
        double vec[1];
        (*table->begin())[field_name].set(vec, vec + 1, 0);
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      // Set the number of records in the table to match the array:
      Index_t num_records = -1;
      try {
        Index_t tmp_num = field_values.size();
        table->setNumRecords(tmp_num);
        num_records = tmp_num;
      } catch (const TipException & x) {
        // Don't report this exception: not testing setNumRecords here.
      }

      if (0 > num_records) {
        ReportWarning("writeFieldTest had trouble preparing to run; test will be skipped!");
      } else if (0 == num_records) {
        ReportWarning("writefieldTest called to write an array with no elements; test will be skipped!");
      } else {
        // Loop over array and table, copying input array into table:
        Table::Iterator out = table->begin();
        for (std::vector<double>::const_iterator itor = field_values.begin(); itor != field_values.end(); ++itor, ++out) {
          (*out)[field_name].set(*itor);
        }
      }
    }
  }

  void TestTable::readWriteVectorFieldTest() {
    if (m_fits_table) {
      Table * table = m_fits_table;
      const Table * const_table = m_fits_table;

      std::string msg;
      std::string vector_field = "cOUnts";

      // Test error cases:
      msg = std::string("getting vector-valued \"") + vector_field + "\" cell into a local scalar variable";
      try {
        // Try to read vector-valued column into a scalar.
        double scalar;
        (*const_table->begin())[vector_field].get(scalar);
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      msg = std::string("setting vector-valued \"") + vector_field + "\" cell from a local scalar variable";
      try {
        // Try to write vector-valued column from a scalar.
        double scalar;
        (*table->begin())[vector_field].set(scalar);
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }

      // Todo: Test success and remove old-style tests.
    }
  }

  Table * TestTable::getTable() {
    return new FitsTable(getDataDir() + "a1.pha", "SPECTRUM", "#row > 0", false);
  }

  // Test appending a field to an existing table.
  void TestTable::appendFieldTest() {
    std::string msg;
    if (0 != m_root_table) {
      msg = "appending field to Root table";
      try {
        m_root_table->appendField("new_chan", "1I");
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }
    }

    if (0 != m_fits_table) {
      msg = "appending field to FITS table";
      try {
        m_fits_table->appendField("NEW_chan", "1I");
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }

      // Error case:
      msg = "appending field which already exists to FITS table";
      try {
        m_fits_table->appendField("new_Chan", "1D");
        ReportUnexpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportExpected(msg + " failed", x);
      }
    }
  }

  void TestTable::copyFieldTest() {
#if MAKE_COMPILATION_FAIL
    throw TipException("SHOULD NOT HAVE COMPILED! Assigning to a ConstTableRecord object");
    Table::ConstIterator itor1;
    Table::ConstIterator itor2;
    *itor1 = *itor2;
#endif
    if (0 != m_fits_table) {
      const Table * in_table = m_fits_table;
      Table * out_table = getTable();
      try {
        // Initialize output table to all zeros.
        setToZero(out_table);

        // Iterate over input and output, copying the input to the output using Cell::operator =.
        Table::ConstIterator in_itor = in_table->begin();
        Table::Iterator out_itor = out_table->begin();
        for (; in_itor != in_table->end(); ++in_itor, ++out_itor) {
          (*out_itor)["channel"] = (*in_itor)["channel"];
          (*out_itor)["counts"] = (*in_itor)["counts"];
        }

        // Confirm the copy worked.
        if (confirmEqual(in_table, out_table)) 
          ReportExpected("copyFieldTest() succeeded copying one table's fields to another using Cell::operator =");
        else
          ReportUnexpected("copyFieldTest() failed to copy one table's fields to another using Cell::operator =");

      } catch (const TipException & x) {
        ReportUnexpected("copyFieldTest() failed", x);
      }

      try {
        // Initialize output table to all zeros.
        setToZero(out_table);

        // Iterate over input and output, copying the input to the output using Record::operator =.
        Table::ConstIterator in_itor = in_table->begin();
        Table::Iterator out_itor = out_table->begin();
        for (; in_itor != in_table->end(); ++in_itor, ++out_itor) {
          (*out_itor) = (*in_itor);
        }

        // Confirm the copy worked.
        if (confirmEqual(in_table, out_table)) 
          ReportExpected("copyFieldTest() succeeded copying one table's fields to another using Record::operator =");
        else
          ReportUnexpected("copyFieldTest() failed to copy one table's fields to another using Record::operator =");

      } catch (const TipException & x) {
        ReportUnexpected("copyFieldTest() failed", x);
      }
      delete out_table;
    }

    // Try copying to a Root file; this should fail.
    if (0 != m_fits_table && 0 != m_root_table) {
      const Table * in_table = m_fits_table;
      try {
        Table::ConstIterator in_itor = in_table->begin();
        Table::Iterator out_itor = m_root_table->begin();

        (*out_itor)["McEnergy"] = (*in_itor)["channel"];

        ReportUnexpected("copyFieldTest() did not fail when copying a cell to a Root file");
      } catch (const TipException & x) {
        ReportExpected("copyFieldTest() failed to copy a cell to a Root file", x);
      }
      try {
        Table::ConstIterator in_itor = in_table->begin();
        Table::Iterator out_itor = m_root_table->begin();

        (*out_itor) = (*in_itor);

        ReportUnexpected("copyFieldTest() did not fail when copying a record to a Root file");
      } catch (const TipException & x) {
        ReportExpected("copyFieldTest() failed to copy a record to a Root file", x);
      }
    }
  }

  void TestTable::setToZero(Table * table) {
    short svalue = 0;
    std::vector<long> vvalue(4096, 0);

    // Zero out columns in output table:
    for (Table::Iterator itor = table->begin(); itor != table->end(); ++itor) {
      (*itor)["Channel"].set(svalue);
      (*itor)["Counts"].set(vvalue);
    }

    // Confirm columns in output table are all zeroes:
    for (Table::Iterator itor = table->begin(); itor != table->end(); ++itor) {
      // Set dummy variable to a non-0 value.
      short svalue = 1;

      // Fetch value from the channel column.
      (*itor)["Channel"].get(svalue);

      // Check whether it's 0.
      if (0 != svalue) throw TipException("setToZero() failed to set all scalar values in a table to 0");

      // Set dummy vector field to non-0 values.
      vvalue.assign(4096, 1);

      // Fetch vector from counts column.
      (*itor)["Counts"].get(vvalue);

      // Check whether vector is now all zeroes.
      for (std::vector<long>::iterator itor = vvalue.begin(); itor != vvalue.end(); ++itor) {
        if (0 != *itor) throw TipException("setToZero() failed to set all vector values in a table to 0");
      }
    }
  }

  bool TestTable::confirmEqual(const Table * table1, const Table * table2) {
    Table::ConstIterator itor1 = table1->begin();
    Table::ConstIterator itor2 = table2->begin();
    for (; itor1 != table1->end(); ++itor1, ++itor2) {
      short chan1 = 0;
      short chan2 = 0;
      std::vector<long> counts1(4096, 0);
      std::vector<long> counts2(4096, 0);

      (*itor1)["channel"].get(chan1);
      (*itor2)["channel"].get(chan2);

      (*itor1)["counts"].get(counts1);
      (*itor2)["counts"].get(counts2);

      if (chan1 != chan2) return false;
      if (counts1 != counts2) return false;
    }
    return true;
  }
}
