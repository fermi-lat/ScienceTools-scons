/** \file TestException.h
    \brief Declaration for class to perform detailed testing of TipException class.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestException_h
#define tip_TestException_h

#include <string>

#include "TestHarness.h"
#include "tip/TipException.h"

namespace tip {

  /** \class TestException
      \brief Declaration for class to perform detailed testing of TipException class.
  */
  class TestException : public TestHarness {
    public:
      /** \brief Construct test objects needed to test Exception class. This will also test Exception's constructor
          in the process.
      */
      TestException() {}

      /** \brief Destructor.
      */
      virtual ~TestException() throw() {}

      /** \brief Perform the detailed test needed by the subobject.
      */
      virtual int test(int status);

  };


  inline int TestException::test(int status) {
    // Use inherited status to set initial status.
    setStatus(status);

    // Make sure errors with no status are reported correctly.
    std::string msg("non-fitsio exception, no status argument");
    std::string correct_msg = msg;
    try {
      throw TipException(msg);
    } catch (const TipException & x) {
        msg = x.what();
      if (correct_msg != msg)
        ReportUnexpected("TestException::test: exception text was \"" + msg + "\", not \"" + correct_msg + "\"");
      else
        ReportExpected("TestException::test: " + correct_msg + " gave correct text");
    }

    msg = "non-fitsio exception, non-zero status argument";
    correct_msg = msg;
    try {
      throw TipException(50, msg);
    } catch (const TipException & x) {
        msg = x.what();
      if (correct_msg != msg)
        ReportUnexpected("TestException::test: exception text was \"" + msg + "\", not \"" + correct_msg + "\"");
      else
        ReportExpected("TestException::test: " + correct_msg + " gave correct text");
    }

    msg = "non-fitsio exception, zero status argument";
    correct_msg = msg;
    try {
      throw TipException(50, msg);
    } catch (const TipException & x) {
        msg = x.what();
      if (correct_msg != msg)
        ReportUnexpected("TestException::test: exception text was \"" + msg + "\", not \"" + correct_msg + "\"");
      else
        ReportExpected("TestException::test: " + correct_msg + " gave correct text");
    }

    msg = "fitsio exception, 104 status argument";
    correct_msg = msg + " (CFITSIO ERROR 104: could not open the named file)";
    try {
      throw TipException(104, msg);
    } catch (const TipException & x) {
        msg = x.what();
      if (correct_msg != msg)
        ReportUnexpected("TestException::test: exception text was \"" + msg + "\", not \"" + correct_msg + "\"");
      else
        ReportExpected("TestException::test: " + correct_msg + " gave correct text");
    }

    return getStatus();
  }

}

#endif
