/** \file TestImage.cxx
    \brief Definition of class to perform detailed testing of Image class.
    \author James Peachey, HEASARC
*/

#include <iostream>
#include <memory>

#include "TestImage.h"
#include "tip/IFileSvc.h"
#include "tip/Image.h"

namespace tip {

  TestImage::TestImage(): m_const_image(0) {}

  TestImage::~TestImage() throw() { delete m_const_image; }

  int TestImage::test(int status) {
    // Use inherited status to set initial status.
    setStatus(status);

    // Test read-only access to individual pixels.
    m_const_image = getConstImage();

    std::vector<PixOrd_t> dims = m_const_image->getImageDimensions();

    for (int ii = dims[0] - 1; ii >= 0; --ii) {
      for (int jj = 0; jj < dims[1]; ++jj) {
        double pixel = 0;
        m_const_image->getPixel(ii, jj, pixel);
        std::cout << pixel << ' ';
      }
      std::cout << std::endl;
    }

    // Test creating a new image and copying the read-only image to it.
    try {
      // Create new image.
      IFileSvc::instance().createFile(std::string("new_image.fits(") + getDataDir() + "arlac.pha)");

      // Open new image for writing.
      std::auto_ptr<Image> image(IFileSvc::instance().editImage("new_image.fits", ""));

      // Flip image X and Y.
      PixOrd_t tmp_dim = dims[0];
      dims[0] = dims[1];
      dims[1] = tmp_dim;

      // Resize image.
      image->setImageDimensions(dims);

      // Copy pixel by pixel from input. Output image will be rotated about the upper-left - lower-right diagonal
      for (int ii = 0; ii < dims[0]; ++ii) {
        for (int jj = 0; jj < dims[1]; ++jj) {
          double pixel = 0;
          m_const_image->getPixel(jj, ii, pixel);
          image->setPixel(dims[0] - 1 - ii, dims[1] - 1 - jj, pixel);
        }
      }

      ReportExpected("TestImage::test did not encounter exception while copying an image");

    } catch (const TipException & x) {
      ReportUnexpected("TestImage::test caught exception ", x);
    }
    return getStatus();

  }

  const Image * TestImage::getConstImage() const {
    return IFileSvc::instance().readImage(getDataDir() + "arlac.pha", "");
  }

}
