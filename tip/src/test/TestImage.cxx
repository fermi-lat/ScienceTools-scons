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
      IFileSvc::instance().createFile("new_image.fits",  getDataDir() + "arlac.pha");

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
      IFileSvc::instance().createFile("new_image2.fits",  getDataDir() + "arlac.pha");

      // Open new image for writing.
      std::auto_ptr<Image> image(IFileSvc::instance().editImage("new_image2.fits", ""));

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

    // Test reading a slice of an image, roughly one half the image cut from the center.
    try {
      // Create new image.
      IFileSvc::instance().createFile("new_image3.fits",  getDataDir() + "arlac.pha");

      // Open new image for writing.
      std::auto_ptr<Image> image(IFileSvc::instance().editImage("new_image3.fits", ""));

      // Create array to read image slice.
      std::vector<float> image_vec;

      // Set limits of slice.
      Image::PixelCoordRange range(2);
      range[0].first = dims[0] / 4;
      range[1].first = dims[1] / 4;
      range[0].second = range[0].first + dims[0] / 2;
      range[1].second = range[1].first + dims[1] / 2;

      // Read slice from input image.
      m_const_image->get(range, image_vec);

      // Recompute dimensions of output image from the slice specification.
      std::vector<PixOrd_t>::iterator d_itor = dims.begin();
      Image::PixelCoordRange::iterator r_itor = range.begin();
      for (; d_itor != dims.end(); ++d_itor, ++r_itor) {
        *d_itor = r_itor->second - r_itor->first;
      }

      // Resize output image.
      image->setImageDimensions(dims);

      // Write slice to output image.
      image->set(image_vec);

      // Confirm that they match up as expected with original image.
      for (int ii = range[0].first; ii < range[0].second; ++ii) {
        for (int jj = range[1].first; jj < range[1].second; ++jj) {
          double orig_pixel = 0.;
          double copy_pixel = 0.;
          m_const_image->getPixel(ii, jj, orig_pixel);
          image->getPixel(ii - range[0].first, jj - range[1].first, copy_pixel);
          if (orig_pixel != copy_pixel)
            throw TipException("After copying a slice of an image, copy does not agree with orig");
        }
      }

      ReportExpected("TestImage::test did not encounter exception while extracting a slice of an image and writing it to a new image");

    } catch (const TipException & x) {
      ReportUnexpected("TestImage::test caught exception ", x);
    }

    // Test writing a slice of an image to have contrived values, roughly one half the image cut from the center.
    try {
      // Open old image for writing.
      std::auto_ptr<Image> image(IFileSvc::instance().editImage("new_image2.fits", ""));

      // Reset dimensions.
      dims = image->getImageDimensions();

      // Set limits of slice.
      Image::PixelCoordRange range(2);
      range[0].first = dims[0] / 4;
      range[1].first = dims[1] / 4;
      range[0].second = range[0].first + dims[0] / 2;
      range[1].second = range[1].first + dims[1] / 2;

      // Compute dimensions of slice from the slice specification.
      long slice_size = 1;
      for (Image::PixelCoordRange::iterator r_itor = range.begin(); r_itor != range.end(); ++r_itor) {
        slice_size *= r_itor->second - r_itor->first;
      }

      // Create array of constant counts to make a square in the center of the image.
      float square_pix = 5.;
      std::vector<float> image_vec(slice_size, square_pix);

      // Write slice to output image.
      image->set(range, image_vec);

      // Confirm that output is the same as input.
      for (int ii = 0; ii < dims[0]; ++ii) {
        for (int jj = 0; jj < dims[1]; ++jj) {
          double orig_pixel = 0.;
          double copy_pixel = 0.;
          image->getPixel(ii, jj, copy_pixel);
          if (ii >= range[0].first && ii < range[0].second && jj >= range[1].first && jj < range[1].second) {
            if (square_pix != copy_pixel)
              throw TipException("A slice of image was not completely overwritten");
          } else {
            m_const_image->getPixel(ii, jj, orig_pixel);
            if (orig_pixel != copy_pixel)
              throw TipException("Part of the image was incorrectly changed by writting a slice");
          }
        }
      }

      ReportExpected("TestImage::test did not encounter exception while changing a slice of an image");

    } catch (const TipException & x) {
      ReportUnexpected("TestImage::test caught exception ", x);
    }
    return getStatus();

  }

  const Image * TestImage::getConstImage() const {
    return IFileSvc::instance().readImage(getDataDir() + "arlac.pha", "");
  }

}
