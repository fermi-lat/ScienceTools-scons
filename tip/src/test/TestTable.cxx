/** \file testTable.cxx
    \brief Definition of class to perform detailed testing of Table class.
    \author James Peachey, HEASARC
*/

#include <cstdlib>

#include "FitsExtensionData.h"
#include "TestTable.h"
#include "tip/Table.h"

namespace tip {

  TestTable::TestTable(): m_data_dir(), m_fits_table(0) {
    std::string msg;
    // Test constructor errors.
    try {
      m_fits_table = new Table(0);
      ReportUnexpected("creating Table object with NULL pointer succeeded");
    } catch(const TipException & x) {
      ReportExpected("creating Table object with NULL pointer failed", x);
    }

    // Find test data directory:
    const char * tiproot = getenv("TIPROOT");
    if (0 != tiproot && 0 != *tiproot) m_data_dir = std::string(tiproot) + "/data/";
    else ReportWarning("TIPROOT environment variable not set");

    // Test constructing a FITS table:
    msg = std::string("opening SPECTRUM extension of ") + m_data_dir + "a1.pha";
    try {
      IExtensionData * data = new FitsExtensionData(m_data_dir + "a1.pha", "SPECTRUM");
      m_fits_table = new Table(data);
      ReportExpected(msg + " succeeded");
    } catch(const TipException & x) {
      ReportUnexpected(msg + " failed");
      ReportWarning("skipping FITS table tests!");
    }
  }

  TestTable::~TestTable() throw() { delete m_fits_table; }

  int TestTable::test(int status) {
    setStatus(status);
    return getStatus();
  }

}
