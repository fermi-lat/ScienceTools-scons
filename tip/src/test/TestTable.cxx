/** \file TestTable.cxx
    \brief Definition of class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "FitsExtensionData.h"
#include "RootExtensionData.h"
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

    // Clean up.
    delete m_root_table; m_root_table = 0;
    delete m_fits_table; m_fits_table = 0;

    return getStatus();
  }

  void TestTable::TableTest() {
    std::string msg;
    // Test constructor errors.
    try {
      m_fits_table = new Table(0);
      ReportUnexpected("creating Table object with NULL pointer succeeded");
      delete m_fits_table; m_fits_table = 0;
    } catch(const TipException & x) {
      ReportExpected("creating Table object with NULL pointer failed", x);
    }

    // Find test data directory:
    std::string data_dir = getDataDir();

    // Test constructing a FITS table:
    msg = std::string("opening SPECTRUM extension of ") + data_dir + "a1.pha";
    try {
      IExtensionData * data = new FitsExtensionData(data_dir + "a1.pha", "SPECTRUM", "", false);
      m_fits_table = new Table(data);
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
      ReportWarning("FITS table tests will be skipped!");
    }

    // Test constructing a Root table:
    msg = std::string("opening TTree \"1\" extension of ") + data_dir + "merit.root";
    try {
      IExtensionData * data = new RootExtensionData(data_dir + "merit.root", "1");
      m_root_table = new Table(data);
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed", x);
      ReportWarning("Root table tests will be skipped!");
    }
  }

  void TestTable::getValidFieldsTest() {
    std::string msg;
    if (0 != m_fits_table) {
      msg = "getting field container from FITS file";
      try {
        // Get container of field names:
        const Table::FieldCont & fields = m_fits_table->getValidFields();
        ReportExpected(msg + " succeeded");

        for (Table::FieldCont::const_iterator it = fields.begin(); it != fields.end(); ++it) {
std::cout << "*************** field " << *it << std::endl;
        }

        try {
        } catch (const TipException & x) {
          ReportUnexpected(msg + " failed", x);
        }

      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
    }
    if (0 != m_root_table) {
      msg = "getting field container from Root file";
      try {
        // Get container of field names:
        const Table::FieldCont & fields = m_root_table->getValidFields();
        ReportExpected(msg + " succeeded");

        for (Table::FieldCont::const_iterator it = fields.begin(); it != fields.end(); ++it) {
std::cout << "*************** field " << *it << std::endl;
        }

        try {
        } catch (const TipException & x) {
          ReportUnexpected(msg + " failed", x);
        }

      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
    }
  }

  void TestTable::readWriteFieldTest() {
    std::string msg;
    std::vector<double> orig;
    std::vector<double> modified;
    std::vector<double> read_modified;
    bool no_error = true;
    if (0 != m_fits_table) {
      // Read original values from table into orig array:
      msg = "testing reading FITS table";
      try {
        readFieldTest(m_fits_table, "channel", orig);
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
      msg = "testing writing FITS table";
      try {
        writeFieldTest(m_fits_table, "channel", modified);
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
        ReportWarning("readWriteFieldTest is skipping some tests");
        no_error = false;
      }

      if (no_error) {
        // Read and check new values from table:
        msg = "testing reading FITS table which was just written";
        try {
          readFieldTest(m_fits_table, "channel", read_modified);

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
      msg = "testing restoring FITS table to its original state";
      try {
        writeFieldTest(m_fits_table, "channel", orig);
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
          readFieldTest(m_fits_table, "channel", read_modified);

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
}
