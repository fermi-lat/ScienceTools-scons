/** \file FitsExtensionData.cxx

    \brief Low level implementation for Fits format tabular data.

    \author James Peachey, HEASARC
*/

#include "table/TableException.h"
#include "FitsExtensionData.h"

namespace table {

  FitsExtensionData::FitsExtensionData(const std::string & file_name, const std::string & ext_name):
    m_file_name(file_name), m_ext_name(ext_name), m_fp(0) {}

  FitsExtensionData::~FitsExtensionData() { close(); }

  void FitsExtensionData::readKeyword(const std::string & field, double & value) const {
    int status = 0;
    fits_read_key(m_fp, TDOUBLE, const_cast<char *>(field.c_str()), &value, 0, &status);
  }

  fitsfile * FitsExtensionData::open() {
    fitsfile * fp = 0;
    int status = 0;

    // Open the fits file.
    fits_open_file(&fp, const_cast<char *>(m_file_name.c_str()), READWRITE, &status);

    if (status) throw TableException();

    // Move to the indicated extension.
    fits_movnam_hdu(fp, BINARY_TBL, const_cast<char *>(m_ext_name.c_str()), 0, &status);

    if (status) {
      fits_close_file(fp, &status);
      throw TableException();
    }

    // Success: save the pointer.
    m_fp = fp;

    return m_fp;
  }

  void FitsExtensionData::close() {
    int status = 0;
    if (m_fp) fits_close_file(m_fp, &status);
    m_fp = 0;
  }

}
