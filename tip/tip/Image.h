/** \file Image.h

    \brief High level encapsulation of image data.

    \author James Peachey, HEASARC
*/
#ifndef tip_Image_h
#define tip_Image_h

#include "tip/Extension.h"
#include "tip/IExtensionData.h"
#include "tip/tip_types.h"

namespace tip {

  /** \class Image

      \brief High level encapsulation of image data.
  */
  class Image : public Extension {
    public:
      Image(IExtensionData * extension_data): Extension(extension_data) {}

      virtual ~Image() {}

      /** \brief Get the dimensionality of an image.
      */
      virtual const std::vector<PixOrd_t> & getImageDimensions() const
        { return m_extension_data->getImageDimensions(); }

      /** \brief Get a specific pixel from an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      void getPixel(PixOrd_t x, PixOrd_t y, double & pixel) const { m_extension_data->getPixel(x, y, pixel); }

      /** \brief Set a specific pixel in an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      void setPixel(PixOrd_t x, PixOrd_t y, const double & pixel) { m_extension_data->setPixel(x, y, pixel); }

  };

}

#endif
