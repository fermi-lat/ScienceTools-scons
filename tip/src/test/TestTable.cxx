/** \file TestTable.cxx
    \brief Definition of class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/

#include <cstdlib>
#include <iostream>

#include "FitsExtensionData.h"
#include "RootExtensionData.h"
#include "TestTable.h"
#include "tip/Table.h"

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
      IExtensionData * data = new FitsExtensionData(data_dir + "a1.pha", "SPECTRUM");
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

}
