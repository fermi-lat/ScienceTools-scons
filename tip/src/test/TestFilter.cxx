/** \file TestFilter.cxx
    \brief Implementation of class to perform detailed testing of Filter abstractions.
    \author James Peachey, HEASARC
*/

#include <iostream>
#include <memory>

#include "TestFilter.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/TipException.h"

namespace tip {

  TestFilter::~TestFilter() throw() {}

  /** \brief Perform all detailed tests.
  */
  int TestFilter::test(int status) {
    setStatus(status);

    // Open a copy of test data file.
    std::auto_ptr<Table> table(IFileSvc::instance().editTable(getDataDir() + "a1.pha", "SPECTRUM", "#row>0"));

    // Verify that the correct number of records is present.
    if (128 != table->getNumRecords())
      ReportUnexpected("before filtering, TestFilter::test found there were " + toString(table->getNumRecords()) +
        " records, not 128");

    // Perform sample filtering.
    std::string filter = "channel>=32 && channel<64";
    table->filterRows(filter);

    // Verify that the correct number of records is present.
    if (32 != table->getNumRecords())
      ReportUnexpected("after applying filter \"" + filter + "\", TestFilter::test found there were " +
        toString(table->getNumRecords()) + " records, not 32");
    else
      ReportExpected("after applying filter \"" + filter + "\", TestFilter::test found there were " +
        toString(table->getNumRecords()) + " records");

    // Try blank filter.
    filter = "";
    table->filterRows(filter);

    // Verify that the correct number of records is still present.
    if (32 != table->getNumRecords())
      ReportUnexpected("after applying filter \"" + filter + "\", TestFilter::test found there were " +
        toString(table->getNumRecords()) + " records, not 32");
    else
      ReportExpected("after applying filter \"" + filter + "\", TestFilter::test found there were " +
        toString(table->getNumRecords()) + " records");

    // Bad filter.
    filter = "invalid=7.";
    try {
      table->filterRows(filter);
      ReportUnexpected("in TestFilter::test, applying filter \"" + filter + "\" did not generate an exception");
    } catch (const TipException & x) {
      ReportExpected("in TestFilter::test, applying filter \"" + filter + "\" generated exception", x);
    }

    return getStatus();
  }

}
