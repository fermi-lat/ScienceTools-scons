/** \file TestFileManager.h
    \brief Declaration for class to perform detailed testing of file manager-related classes.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestFileManager_h
#define tip_TestFileManager_h

#include "TestHarness.h"

namespace tip {

  /** \class TestFileManager
      \brief Declaration for class to perform detailed testing of file manager-related classes.
  */
  class TestFileManager : public TestHarness {
    public:
      /** \brief Perform the detailed test needed by the subobject.
      */
      virtual int test(int status);

      /// \brief Test createFile method.
      void createFileTest();

      /// \brief Test editExtension method.
      void editExtensionTest();

      /// \brief Test readExtension method.
      void readExtensionTest();

      /// \brief Test readTable method.
      void readTableTest();

      /// \brief Test file status/attributes methods.
      void fileStatusTest();

      /// \brief Test updating all keywords in a file.
      void updateKeywordsTest();

      /// \brief Test appendImage method.
      void appendImageTest();

      /// \brief Test appendImage method.
      void appendTableTest();
  };

}

#endif
