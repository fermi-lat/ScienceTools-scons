/** \file TestFilter.h
    \brief Declaration for class to perform detailed testing of Filter abstractions.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestFilter_h
#define tip_TestFilter_h

#include "TestHarness.h"

namespace tip {

  /** \class TestFilter
      \brief Declaration for class to perform detailed testing of Filter abstractions.
  */
  class TestFilter : public TestHarness {
    public:
      /** \brief Destructor.
      */
      virtual ~TestFilter() throw();

      /** \brief Perform all detailed tests.
      */
      virtual int test(int status);
  };

}

#endif
