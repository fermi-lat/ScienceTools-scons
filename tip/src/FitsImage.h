/** \file FitsImage.h

    \brief Utilities to help manage FITS specific table access. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef tip_FitsImage_h
#define tip_FitsImage_h

#include <string>
#include <vector>

#include "fitsio.h"

#include "FitsHeader.h"
#include "FitsPrimProps.h"
#include "tip/Image.h"
#include "tip/tip_types.h"

namespace tip {

  /** \class FitsImage

      \brief Low level interface to FITS format extensions. This is not part of the API.

      This class is a standalone utility class which encapsulates Cfitsio (fitsfile *) access. It also
      acts as a factory for creating FITS-specific header and data objects, which refer back to the
      FitsImage object which created them.
  */
  class FitsImage : public Image {
    public:
      /** \brief Create an object to provide low-level access to the given FITS extension.
          \param file_name The name of the FITS file.
          \param ext_name The name of the FITS extension.
      */
      FitsImage(const std::string & file_name, const std::string & ext_name,
        const std::string & filter = "", bool read_only = true);

      /** \brief Destructor. Closes image if it is open.
      */
      virtual ~FitsImage();

      void close(int status = 0);

      /** \brief Retrieve Header object, which is a container of FITS-like keywords, non-const version.
      */
      virtual Header & getHeader();

      /** \brief Retrieve Header object, which is a container of FITS-like keywords, const version.
      */
      virtual const Header & getHeader() const;

      /** \brief Returns true if the extension is a image, false otherwise.
      */
      virtual bool isImage() const { return true; }

      /** \brief Returns true if the extension is a table, false otherwise.
      */
      virtual bool isTable() const { return false; }

      /** \brief Get the dimensionality of an image.
      */
      virtual const std::vector<PixOrd_t> & getImageDimensions() const;

      /** \brief Set the dimensionality of an image.
          \param dims Array holding the sizes in each dimension.
      */
      virtual void setImageDimensions(const std::vector<PixOrd_t> & dims);

      /** \brief Get a specific pixel from an image extension.
          \param x Vector of coordinates of the pixel.
          \param pixel The pixel value.
      */
      virtual void getPixel(const std::vector<PixOrd_t> & x, double & pixel) const;

      /** \brief Set a specific pixel in an image extension.
          \param x Vector of coordinates of the pixel.
          \param pixel The pixel value.
      */
      virtual void setPixel(const std::vector<PixOrd_t> & x, const double & pixel);

      /** \brief Get an entire image, regardless of its dimensionality, as a one-dimensional array.
          \param image The array in which to store the image.
      */
      virtual void get(std::vector<float> & image) const;

      /** \brief Get an entire image, regardless of its dimensionality, as a one-dimensional array.
          \param image The array which stores the image.
      */
      virtual void set(const std::vector<float> & image);

      fitsfile * getFp() const { return m_header.getFp(); }

      bool readOnly() const { return m_header.readOnly(); }

    protected:
      /** \brief Open a FITS image. Exceptions will be thrown if the extension does not exist, or if
          the extension is not an image. Normally this is called by open()
      */
      void openImage();

    private:
      std::string formatWhat(const std::string & msg) const;

      FitsHeader m_header;
      std::string m_file_name;
      std::string m_ext_name;
      std::string m_filter;
      std::vector<PixOrd_t> m_image_dimensions;
  };

}

#endif
