/** \file TestImage.cxx
    \brief Definition of class to perform detailed testing of Image class.
    \author James Peachey, HEASARC
*/

#include <iostream>

#include "TestImage.h"
#include "tip/IFileSvc.h"
#include "tip/Image.h"

namespace tip {

  TestImage::TestImage(): m_const_image(0) {}

  TestImage::~TestImage() throw() { delete m_const_image; }

  int TestImage::test(int status) {
    // Use inherited status to set initial status.
    setStatus(status);

    m_const_image = getConstImage();

    const std::vector<PixOrd_t> & dims = m_const_image->getImageDimensions();

    for (int ii = dims[0] - 1; ii >= 0; --ii) {
      for (int jj = 0; jj < dims[1]; ++jj) {
        double pixel = 0;
        m_const_image->getPixel(ii, jj, pixel);
        std::cout << pixel << ' ';
      }
      std::cout << std::endl;
    }
    return getStatus();
  }

  const Image * TestImage::getConstImage() const {
    return IFileSvc::instance().readImage(getDataDir() + "arlac.pha", "");
  }

}
