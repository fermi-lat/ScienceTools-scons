/** \file TestKeyRecord.h
    \brief Declaration for class to perform detailed testing of KeyRecord abstractions.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestKeyRecord_h
#define tip_TestKeyRecord_h

#include "TestHarness.h"

namespace tip {

  /** \class TestKeyRecord
      \brief Declaration for class to perform detailed testing of KeyRecord abstractions.
  */
  class TestKeyRecord : public TestHarness {
    public:
      /** \brief Destructor.
      */
      virtual ~TestKeyRecord() throw();

      /** \brief Perform all detailed tests.
      */
      virtual int test(int status);

      std::string formatRec(const std::string & name, const std::string & value, const std::string & comment);

      std::string formatStringRec(const std::string & name, const std::string & value, const std::string & comment);
  };

}

#endif
