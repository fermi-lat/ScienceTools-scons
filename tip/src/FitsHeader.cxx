/** \file FitsHeader.cxx

    \brief Implementation of utilities to help manage FITS specific table access.

    \author James Peachey, HEASARC
*/
#include <sstream>

#include "FitsHeader.h"
#include "tip/TipException.h"

namespace tip {

  FitsHeader::FitsHeader(const std::string & file_name, const std::string & ext_name,
    const std::string & filter, bool read_only): m_file_name(file_name), m_ext_name(ext_name),
    m_filter(filter), m_fp(0), m_is_table(false), m_read_only(read_only) { open(); }

  // Close file automatically while destructing.
  FitsHeader::~FitsHeader() { close(); }

  // Subclasses call this to open the file and position it to the desired extension.
  void FitsHeader::open() {
    if (0 == m_fp) {
      fitsfile * fp = 0;
      int status = 0;

      // Construct the full name of the file from file name [extension] [filter] (if any):
      std::ostringstream s;
      s << m_file_name;
      if (!m_ext_name.empty()) s << "[" << m_ext_name << "]";
      if (!m_filter.empty()) s << "[" << m_filter << "]";
      std::string file_name = s.str();

      // Try to open the fits file read-write, unless read-only mode was explicitly set before open
      // was called.
      if (!m_read_only)
        fits_open_file(&fp, const_cast<char *>(file_name.c_str()), READWRITE, &status);

      // If opening read-write didn't work, or if read-only mode was explicitly set before open
      // was called...
      if (0 != status || m_read_only) {
        // Attempt to open the file read-only:
        status = 0;
        fits_open_file(&fp, const_cast<char *>(file_name.c_str()), READONLY, &status);
        m_read_only = true;
      }

      if (0 != status) {
        // TODO 9. 4/2/2004: Bug in cfitsio 2.48: Check for it and warn about it. The bug causes
        // the parser not to move to the correct extension.
        // DONE 9: 4/21/2004: A warning is issued for this specific version. Nothing more can be done, really.
        float cfitsio_version = 0.;
        fits_get_version(&cfitsio_version);
        // This is surreal. A FLOATING POINT VERSION NUMBER! Checking for == doesn't work -- I tried it.
        if (2.4795 < cfitsio_version && 2.4805 > cfitsio_version)
          throw TipException(std::string("WARNING: there is a known bug in Cfitsio 2.48's extended "
            "syntax parser!\nCould not open FITS extension ") + file_name);
        throw TipException(std::string("Could not open FITS extension \"") + file_name + '"');
      }

      // Success: save the pointer.
      m_fp = fp;

      // Check whether the file pointer is pointing at a table:
      int hdu_type = 0;
      fits_get_hdu_type(m_fp, &hdu_type, &status);
      if (0 != status) {
        close(status);
        throw TipException(formatWhat("Could not determine the type of the HDU"));
      }
      if (ASCII_TBL == hdu_type || BINARY_TBL == hdu_type)
        m_is_table = true;
    }
  }

  // Close file.
  void FitsHeader::close(int status) {
    if (0 != m_fp) fits_close_file(m_fp, &status);
    m_fp = 0;
  }

  std::string FitsHeader::formatWhat(const std::string & msg) const {
    std::ostringstream msg_str;
    msg_str << msg;
    if (!m_ext_name.empty()) msg_str << " in extension \"" << m_ext_name << '"';
    msg_str << " in file \"" << m_file_name << '"';
    return msg_str.str();
  }

}
