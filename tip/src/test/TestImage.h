/** \file TestImage.h
    \brief Declaration for class to perform detailed testing of Image class.
    \author James Peachey, HEASARC
*/
#ifndef tip_TestImage_h
#define tip_TestImage_h

#include "TestHarness.h"

namespace tip {

  class Image;

  /** \class TestImage
      \brief Declaration for class to perform detailed testing of Image class.
  */
  class TestImage : public TestHarness {
    public:
      /** \brief Constructor.
      */
      TestImage();
      /** \brief Destructor.
      */
      virtual ~TestImage() throw();

      /** \brief Perform the detailed test needed by the subobject.
      */
      virtual int test(int status);

      /** \brief Get test image.
      */
      const Image * getConstImage() const;

    private:
      const Image * m_const_image;
  };

}

#endif
