/** \file TestExtensionData.h
    \brief Declaration for class to perform detailed testing of extension data classes.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestExtensionData_h
#define tip_TestExtensionData_h

#include "TestHarness.h"

namespace tip {

  class IExtensionData;

  /** \class TestExtensionData
      \brief Declaration for class to perform detailed testing of extension data classes.
  */
  class TestExtensionData : public TestHarness {
    public:
      /** \brief Construct test objects needed to test extension data classes.
      */
      TestExtensionData();

      /** \brief Destructor.
      */
      virtual ~TestExtensionData() throw();

      /** \brief Perform all detailed tests.
      */
      virtual int test(int status);

      /** \brief Test read-only file access.
      */
      void testReadOnly();

      /** \brief Confirm that the given object can only be accessed read-only.
      */
      void confirmReadOnly(IExtensionData * extension);

    private:
    IExtensionData * m_read_only_extension;
  };

}

#endif
