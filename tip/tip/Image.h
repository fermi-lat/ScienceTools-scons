/** \file Image.h

    \brief High level encapsulation of image data.

    \author James Peachey, HEASARC
*/
#ifndef tip_Image_h
#define tip_Image_h

#include <vector>

#include "tip/Extension.h"
#include "tip/tip_types.h"

namespace tip {

  /** \class Image

      \brief High level encapsulation of image data.
  */
  class Image : public Extension {
    public:
      /** \brief Destructor. Closes image if it is open.
      */
      virtual ~Image() {}

      /** \brief Get the dimensionality of an image.
      */
      virtual const std::vector<PixOrd_t> & getImageDimensions() const = 0;

      /** \brief Get the dimensionality of an image.
      */
      virtual void setImageDimensions(const std::vector<PixOrd_t> & dims) = 0;

      /** \brief Get a specific pixel from an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      void getPixel(PixOrd_t x, PixOrd_t y, double & pixel) const {
        std::vector<long> coord(2);
        coord[0] = x;
        coord[1] = y;
        getPixel(coord, pixel);
      }

      /** \brief Get a specific pixel from an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      virtual void getPixel(const std::vector<PixOrd_t> & x, double & pixel) const = 0;

      /** \brief Set a specific pixel in an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      void setPixel(PixOrd_t x, PixOrd_t y, const double & pixel) {
        std::vector<long> coord(2);
        coord[0] = x;
        coord[1] = y;
        setPixel(coord, pixel);
      }

      /** \brief Set a specific pixel in an image extension.
          \param x The x ordinate of the pixel.
          \param y The y ordinate of the pixel.
          \param pixel The pixel value.
      */
      virtual void setPixel(const std::vector<PixOrd_t> & x, const double & pixel) = 0;

      /** \brief Get an entire image, regardless of its dimensionality, as a one-dimensional array.
          \param image The array in which to store the image.
      */
      virtual void get(std::vector<float> & image) const = 0;

      /** \brief Get an entire image, regardless of its dimensionality, as a one-dimensional array.
          \param image The array which stores the image.
      */
      virtual void set(const std::vector<float> & image) = 0;
  };

}

#endif
