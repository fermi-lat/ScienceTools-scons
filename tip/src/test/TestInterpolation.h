/** \file TestInterpolation.h
    \brief Declaration for class to perform detailed testing of Interpolation abstractions.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestInterpolation_h
#define tip_TestInterpolation_h

#include <string>

#include "TestHarness.h"

namespace tip {

  /** \class TestInterpolation
      \brief Declaration for class to perform detailed testing of Interpolation abstractions.
  */
  class TestInterpolation : public TestHarness {
    public:
      /** \brief Destructor.
      */
      virtual ~TestInterpolation() throw();

      /** \brief Perform all detailed tests.
      */
      virtual int test(int status);
  };

}

#endif
