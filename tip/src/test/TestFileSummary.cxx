/** \file TestFileSummary.cxx
    \brief Implementation for class to perform detailed testing of data file abstractions.
    \author James Peachey, HEASARC
*/
#include "TestFileSummary.h"
#include "tip/FileSummary.h"
#include "tip/IFileSvc.h"
#include "tip/TipException.h"

namespace tip {

  int TestFileSummary::test(int status) {
    setStatus(status);
    std::string msg;

    try {
      msg = "creating data file summary";

      // Create file summary object.
      FileSummary summary;

      // Fill summary object with information using IFileSvc.
      IFileSvc::instance().getFileSummary(getDataDir() + "arlac.pha", summary);

      ReportExpected(msg + " succeeded");

      try {
        msg = "iterating over data file summary";

        // Iterate over extensions in that file, and make sure they have the expected names.
        if (3u != summary.size()) ReportUnexpected("test data file does not have expected number of extensions.");
        else {
          // Known contents of test file are:
          const char * expected [] = { "0", "SPECTRUM", "GTI" };

          for (int index = 0; index < 3; ++index) {
            if (0 != summary[index].getExtId().compare(expected[index]))
              ReportUnexpected(std::string("Extension number ") + toString(index) + " has id " + summary[index].getExtId() +
                " not " + expected[index]);
          }
        }
        ReportExpected(msg + " succeeded");
      } catch (const TipException & x) {
        ReportUnexpected(msg + " failed", x);
      }
    } catch (const TipException & x) {
      ReportUnexpected(msg + " failed", x);
    }

    return getStatus();
  }

}
