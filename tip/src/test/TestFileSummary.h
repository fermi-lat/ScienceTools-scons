/** \file TestFileSummary.h
    \brief Declaration for class to perform detailed testing of data file abstractions.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestFileSummary_h
#define tip_TestFileSummary_h

#include "TestHarness.h"

namespace tip {

  /** \class TestFileSummary
      \brief Declaration for class to perform detailed testing of data file abstractions.
  */
  class TestFileSummary : public TestHarness {
    public:
      /** \brief Perform all detailed tests.
      */
      virtual int test(int status);
  };

}

#endif
