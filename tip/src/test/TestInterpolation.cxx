/** \file TestInterpolation.cxx
    \brief Implementation of class to perform detailed testing of Interpolation abstractions.
    \author James Peachey, HEASARC
*/
#include <memory>
#include <vector>

#include "TestInterpolation.h"
#include "tip/IFileSvc.h"
#include "tip/LinearInterp.h"
#include "tip/Table.h"

namespace tip {

  TestInterpolation::~TestInterpolation() throw() {}

  /** \brief Perform all detailed tests.
  */
  int TestInterpolation::test(int status) {
    setStatus(status);

    std::auto_ptr<const Table> table(IFileSvc::instance().readTable(getDataDir() + "a1.pha", "SPECTRUM"));

    LinearInterp interp(table->begin(), table->end());

    // Error cases first.
    try {
      interp.interpolate("TIME", -1.);
      ReportUnexpected("interpolating non-existent field succeeded");
    } catch (const std::exception & x) {
      ReportExpected("interpolating non-existent field failed", x);
    }

    try {
      interp.interpolate("CHANNEL", -1.);
      ReportUnexpected("interpolating channel field before first value succeeded");
    } catch (const std::exception & x) {
      ReportExpected("interpolating channel field before first value failed", x);
    }

    try {
      interp.interpolate("CHANNEL", 4098.);
      ReportUnexpected("interpolating channel field after last value succeeded");
    } catch (const std::exception & x) {
      ReportExpected("interpolating channel field after last value failed", x);
    }

    // Successes.
    try {
      // Very first value in range should interpolate successfully.
      interp.interpolate("CHANNEL", 0.);

      double channel = interp.get("CHANNEL");
      if (0. != channel)
        ReportUnexpected("interpolating channel field with value == first value did not return value == first value");
      else
        ReportExpected("interpolating channel field with value == first value returned value == first value");

      // Check vector interpolation.
      std::vector<double> counts0_vec;
      (*table->begin())["COUNTS"].get(counts0_vec);

      std::vector<double> counts_vec;
      interp.get("COUNTS", counts_vec);

      if (counts0_vec != counts_vec) {
        ReportUnexpected("interpolating counts field with value == first value did not return value == first value");
      } else
        ReportExpected("interpolating counts field with value == first value returned value == first value");

      // Arbitrary value should re-interpolate back to itself.
      interp.interpolate("CHANNEL", 77.333);

      channel = interp.get("CHANNEL");
      if (77.333 != channel)
        ReportUnexpected("interpolating channel field for 77.333 did not compute proper value");
      else
        ReportExpected("interpolating channel field for 77.333 computed proper value");

      interp.get("CHANNEL");
      if (77.333 != channel)
        ReportUnexpected("interpolating channel field for 77.333 did not compute proper value");
      else
        ReportExpected("interpolating channel field for 77.333 computed proper value");

    } catch (const std::exception & x) {
      ReportUnexpected("interpolating channel field with value == first value succeeded", x);
    }

    return getStatus();
  }

}
