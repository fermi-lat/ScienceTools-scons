/** \file FitsExtension.cxx

    \brief Low level implementation for Fits format tabular data.

    \author James Peachey, HEASARC
*/

#include "FitsExtension.h"
#include "table/TableException.h"

namespace table {

  FitsExtension::FitsExtension(const std::string & file_name, const std::string & ext_name):
    m_file_name(file_name), m_ext_name(ext_name), m_fp(0) {}

  FitsExtension::~FitsExtension() { close(); }

  fitsfile * FitsExtension::open() {
    fitsfile * fp = 0;
    int status = 0;

    // Open the fits file.
    fits_open_file(&fp, const_cast<char *>(m_file_name.c_str()), READWRITE, &status);

    if (status) throw TableException();

    // Move to the indicated extension.
    fits_movnam_hdu(fp, ANY_HDU, const_cast<char *>(m_ext_name.c_str()), 0, &status);

    if (status) {
      fits_close_file(fp, &status);
      throw TableException();
    }

    // Success: save the pointer.
    m_fp = fp;

    return m_fp;
  }

  void FitsExtension::close() {
    int status = 0;
    if (m_fp) fits_close_file(m_fp, &status);
    m_fp = 0;
  }

}
