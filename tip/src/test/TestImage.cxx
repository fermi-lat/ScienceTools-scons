/** \file TestImage.cxx
    \brief Definition of class to perform detailed testing of Image class.
    \author James Peachey, HEASARC
*/

#include <iostream>
#include <memory>
#include <vector>

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

    // Write the pixel values to the screen. Go row-by-row, so use dims1 for outer loop.
    // Reverse the order of the rows, because FITS images have row 1 at the bottom.
    for (int jj = dims[1] - 1; jj >= 0; --jj) {
      for (int ii = 0; ii < dims[0]; ++ii) {
        double pixel = 0;
        m_const_image->getPixel(ii, jj, pixel);
        std::cout << pixel << ' ';
      }
      std::cout << std::endl;
    }

    // Test creating a new image and copying the read-only image to it, pixel by pixel.
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

      // Verify that the size was changed.
      if (image->getImageDimensions() != dims)
        ReportUnexpected("TestImage::test: after setImageDimensions, getImageDimensions returned a different set of dimensions");

      // Copy pixel by pixel from input. Output image will be rotated about the upper-left - lower-right diagonal
      for (int ii = 0; ii < dims[0]; ++ii) {
        for (int jj = 0; jj < dims[1]; ++jj) {
          double pixel = 0;
          m_const_image->getPixel(jj, ii, pixel);
          image->setPixel(dims[0] - 1 - ii, dims[1] - 1 - jj, pixel);
        }
      }

      // Confirm that output is the same as input.
      for (int ii = 0; ii < dims[0]; ++ii) {
        for (int jj = 0; jj < dims[1]; ++jj) {
          double orig_pixel = 0.;
          double copy_pixel = 0.;
          m_const_image->getPixel(jj, ii, orig_pixel);
          image->getPixel(dims[0] - 1 - ii, dims[1] - 1 - jj, copy_pixel);
          if (orig_pixel != copy_pixel) throw TipException("After copying an image pixel-by-pixel, copy does not agree with orig");
        }
      }

      ReportExpected("TestImage::test did not encounter exception while copying an image pixel by pixel");

    } catch (const TipException & x) {
      ReportUnexpected("TestImage::test caught exception ", x);
    }

    // Test creating a new image and copying the read-only image to it in one fell swoop.
    try {
      // Create new image.
      IFileSvc::instance().createFile(std::string("new_image.fits(") + getDataDir() + "arlac.pha)");

      // Open new image for writing.
      std::auto_ptr<Image> image(IFileSvc::instance().editImage("new_image.fits", ""));

      // Create array for read image.
      std::vector<float> image_vec;

      // Read entire image from input.
      m_const_image->get(image_vec);

      // Copy it to the output.
      image->set(image_vec);

      // Re-read the image dimensions.
      dims = m_const_image->getImageDimensions();

      // Confirm that output is the same as input.
      for (int ii = 0; ii < dims[0]; ++ii) {
        for (int jj = 0; jj < dims[1]; ++jj) {
          double orig_pixel = 0.;
          double copy_pixel = 0.;
          m_const_image->getPixel(ii, jj, orig_pixel);
          image->getPixel(ii, jj, copy_pixel);
          if (orig_pixel != copy_pixel)
throw TipException("After copying a whole image, copy does not agree with orig");
        }
      }

      ReportExpected("TestImage::test did not encounter exception while copying a whole image at one time");

    } catch (const TipException & x) {
      ReportUnexpected("TestImage::test caught exception ", x);
    }
    return getStatus();

  }

  const Image * TestImage::getConstImage() const {
    return IFileSvc::instance().readImage(getDataDir() + "arlac.pha", "");
  }

}
