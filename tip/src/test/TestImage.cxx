/** \file TestImage.cxx
    \brief Definition of class to perform detailed testing of Image class.
    \author James Peachey, HEASARC
*/

#include <iostream>

#include "TestImage.h"
#include "FitsExtensionData.h"
#include "tip/Image.h"

namespace tip {

  TestImage::TestImage(): m_image(0) {}

  TestImage::~TestImage() throw() { delete m_image; }

  int TestImage::test(int status) {
    // Use inherited status to set initial status.
    setStatus(status);

    m_image = getImage();

    const std::vector<PixOrd_t> & dims = m_image->getImageDimensions();

    for (int ii = dims[0] - 1; ii >= 0; --ii) {
      for (int jj = 0; jj < dims[1]; ++jj) {
        double pixel = 0;
        m_image->getPixel(ii, jj, pixel);
        std::cout << pixel << ' ';
      }
      std::cout << std::endl;
    }
    return getStatus();
  }

  Image * TestImage::getImage() const {
    IExtensionData * data = new FitsExtensionData(getDataDir() + "arlac.pha", "");
    return new Image(data);
  }

}
