/** \file TestExtensionData.h
    \brief Declaration for class to perform detailed testing of extension data classes.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestExtensionData_h
#define tip_TestExtensionData_h

#include "TestHarness.h"

namespace tip {

  class Image;
  class Table;

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
      void confirmReadOnly(Table * extension);

      /** \brief Test functions which change the extension object..
      */
      void testReadWrite();

      /** \brief Test copying an extension object.
      */
      void testCopy();

    private:
    Table * m_read_only_extension;
    Table * m_writable_extension;
  };

}

#endif
