/** \file FitsImage.cxx

    \brief Implementation of utilities to help manage FITS specific table access.

    \author James Peachey, HEASARC
*/
#include <sstream>

#include "FitsImage.h"
#include "tip/TipException.h"

namespace tip {

  FitsImage::FitsImage(const std::string & file_name, const std::string & ext_name,
    const std::string & filter, bool read_only): m_header(file_name, ext_name, filter, read_only),
    m_file_name(file_name), m_filter(filter), m_image_dimensions() { openImage(); }

  // Close file automatically while destructing.
  FitsImage::~FitsImage() { close(); }

  // Close file.
  void FitsImage::close(int status) {
    m_image_dimensions.clear();
    m_header.close(status);
  }

  Header & FitsImage::getHeader() { return m_header; }

  const Header & FitsImage::getHeader() const { return m_header; }

  const std::string & FitsImage::getName() const { return m_header.getName(); }

  void FitsImage::setName(const std::string & name) { m_header.setName(name); }

  const std::vector<PixOrd_t> & FitsImage::getImageDimensions() const {
    return m_image_dimensions;
  }

  void FitsImage::setImageDimensions(const std::vector<PixOrd_t> & dims) {
    // Make C primitive copy of array to pass to Cfitsio.
    std::vector<PixOrd_t>::size_type naxis = dims.size();
    std::vector<long> naxes(dims.begin(), dims.end());

    int status = 0;
    int bitpix = 0;

    // Get current image type, which will not be changed by the resize operation.
    fits_get_img_type(m_header.getFp(), &bitpix, &status);
    if (0 != status) throw TipException(status, formatWhat("setImageDimensions cannot determine image type"));

    // Resize the image.
    fits_resize_img(m_header.getFp(), bitpix, naxis, &*naxes.begin(), &status);
    if (0 != status) throw TipException(status, formatWhat("setImageDimensions cannot change image dimensions"));

    // Save the dimensions in the dimension member.
    m_image_dimensions = dims;
  }

  void FitsImage::getPixel(const PixelCoordinate & coord, double & pixel) const {
    int status = 0;
    // Make a copy of coordinates for cfitsio to use.
    std::vector<long> cf_coord(coord.size());

    // Cfitsio starts numbering at 1 not 0.
    for (PixelCoordinate::size_type index = 0; index != cf_coord.size(); ++index) cf_coord[index] = coord[index] + 1;

    double array[2] = { 0., 0. };

    // Read the given pixel:
    fits_read_pix(m_header.getFp(), TDOUBLE, &*cf_coord.begin(), 1, 0, array, 0, &status);
    if (0 != status) throw TipException(status, formatWhat("getPixel could not read pixel as a double"));

    // Copy the value just read:
    pixel = *array;
  }

  void FitsImage::setPixel(const PixelCoordinate & coord, const double & pixel) {
    if (m_header.readOnly()) throw TipException(formatWhat("setPixel called for read-only image"));
    int status = 0;
    // Make a copy of coordinates for cfitsio to use.
    std::vector<long> cf_coord(coord.size());

    // Cfitsio starts numbering at 1 not 0.
    for (PixelCoordinate::size_type index = 0; index != cf_coord.size(); ++index) cf_coord[index] = coord[index] + 1;

    // Copy pixel into temporary array:
    double array[2] = { pixel, 0. };

    // Write the copy to the output file:
    fits_write_pix(m_header.getFp(), TDOUBLE, &*cf_coord.begin(), 1, array, &status);
    if (0 != status) throw TipException(status, formatWhat("setPixel could not write a double to a pixel"));
  }

  void FitsImage::get(std::vector<float> & image) const {
    int status = 0;

    // Compute overall size of image.
    unsigned long image_size = 1;
    for (std::vector<PixOrd_t>::const_iterator itor = m_image_dimensions.begin(); itor != m_image_dimensions.end(); ++itor)
      image_size *= *itor;

    // Make sure image is large enough to accomodate the image.
    image.resize(image_size);

    // Starting coordinate is the first pixel in each dimension.
    std::vector<long> coord(m_image_dimensions.size(), 1);

    // Get the image itself.
    fits_read_pix(m_header.getFp(), TFLOAT, &*coord.begin(), image_size, 0, &*image.begin(), 0, &status);
  }

  void FitsImage::get(const PixelCoordRange & range, std::vector<float> & image) const {
    int status = 0;

    // Create arrays which contain first and last pixel in cfitsio's indexing scheme.
    std::vector<long> fpixel(range.size());
    std::vector<long> lpixel(range.size());

    // Interpret range to get arrays of first and last pixels, as well as the total image size.
    // Note: Correct for fact that lpixel is indexed starting with 1 not 0:
    //   fpixel = begin_pixel + 1 (offset for indexing)
    // However, for lpixel there is a second correction because end_pixel is defined as one past the last pixel. So:
    //   lpixel = end_pixel + 1 (offset for indexing) - 1 (end_pixel is one pixel past last pixel) = end_pixel
    // Thus, these corrections offset, and so there is no correction for lpixel.
    long slice_size = 1;
    for (PixelCoordRange::size_type index = 0; index != range.size(); ++index) {
      fpixel[index] = range[index].first + 1; // Cfitsio indexes image coordinates starting with 1 not 0.
      lpixel[index] = range[index].second; // DO NOT add 1, because range already is 1 past the last pixel.
      slice_size *= range[index].second - range[index].first;
    }

    // Resize image array.
    image.resize(slice_size);

    // Set up the down sampling array: skip no pixels.
    std::vector<long> inc(fpixel.size(), 1);

    // Get the image.
    fits_read_subset(m_header.getFp(), TFLOAT, &*fpixel.begin(), &*lpixel.begin(), &*inc.begin(), 0, &*image.begin(), 0, &status);
    if (0 != status) throw TipException(status, formatWhat("could not read image subset"));
  }

  void FitsImage::set(const std::vector<float> & image) {
    int status = 0;

    // Compute overall size of image.
    unsigned long image_size = 1;
    for (std::vector<PixOrd_t>::const_iterator itor = m_image_dimensions.begin(); itor != m_image_dimensions.end(); ++itor)
      image_size *= *itor;

    // Make sure no more than the image_size elements are written.
    image_size = (image_size < image.size()) ? image_size : image.size();

    // Starting coordinate is the first pixel in each dimension.
    std::vector<long> coord(m_image_dimensions.size(), 1);

    // Write the image itself.
    fits_write_pix(m_header.getFp(), TFLOAT, &*coord.begin(), image_size, const_cast<float *>(&*image.begin()), &status);
  }

  void FitsImage::set(const PixelCoordRange & range, const std::vector<float> & image) {
    int status = 0;

    // Create arrays which contain first and last pixel in cfitsio's indexing scheme.
    std::vector<long> fpixel(range.size());
    std::vector<long> lpixel(range.size());

    // Interpret range to get arrays of first and last pixels, as well as the total image size.
    // Note: Correct for fact that lpixel is indexed starting with 1 not 0:
    //   fpixel = begin_pixel + 1 (offset for indexing)
    // However, for lpixel there is a second correction because end_pixel is defined as one past the last pixel. So:
    //   lpixel = end_pixel + 1 (offset for indexing) - 1 (end_pixel is one pixel past last pixel) = end_pixel
    // Thus, these corrections offset, and so there is no correction for lpixel.
    for (PixelCoordRange::size_type index = 0; index != range.size(); ++index) {
      fpixel[index] = range[index].first + 1; // Cfitsio indexes image coordinates starting with 1 not 0.
      lpixel[index] = range[index].second; // DO NOT add 1, because range already is 1 past the last pixel.
    }

    // Write the image itself.
    fits_write_subset(m_header.getFp(), TFLOAT, &*fpixel.begin(), &*lpixel.begin(), const_cast<float *>(&*image.begin()), &status);
  }

  void FitsImage::openImage() {
    // Check whether the file pointer is pointing at a table:
    if (m_header.isTable()) {
      close();
      throw TipException(formatWhat("HDU is not an image"));
    }

    int naxis = 0;
    int status = 0;

    // Get number of axes:
    fits_get_img_dim(m_header.getFp(), &naxis, &status);
    if (0 != status) throw TipException(status, formatWhat("Cannot get number of dimensions of image"));

    m_image_dimensions.clear();

    // Get naxes:
    std::vector<long> naxes(naxis);
    fits_get_img_size(m_header.getFp(), naxis, &*naxes.begin(), &status);
    if (0 != status) {
      throw TipException(status, formatWhat("Cannot get dimensions of each degree of freedom of image"));
    }

    m_image_dimensions.assign(naxes.begin(), naxes.end());
  }

  std::string FitsImage::formatWhat(const std::string & msg) const {
    std::ostringstream msg_str;
    msg_str << msg;
    const std::string & ext_name(getName());
    if (!ext_name.empty()) msg_str << " in extension \"" << ext_name << '"';
    msg_str << " in file \"" << m_file_name << '"';
    return msg_str.str();
  }

}
