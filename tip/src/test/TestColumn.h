/** \file TestColumn.h
    \brief Declaration for class to perform detailed testing of column abstractions.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestColumn_h
#define tip_TestColumn_h

#include <string>

#include "TestHarness.h"

namespace tip {

  /** \class TestColumn
      \brief Declaration for class to perform detailed testing of column abstractions.
  */
  class TestColumn : public TestHarness {
    public:
      /** \brief Destructor.
      */
      virtual ~TestColumn() throw();

      /** \brief Perform all detailed tests.
      */
      virtual int test(int status);

      void copyDataFile(const std::string & in_file, const std::string & out_file);
  };

}

#endif
