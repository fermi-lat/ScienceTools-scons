/** \file FitsFileManager.cxx

    \brief Utilities to help manage FITS files. These classes are not part of the API.

    \author James Peachey, HEASARC
*/

#include <sstream>

#include "FitsFileManager.h"
#include "fitsio.h"
#include "tip/TipException.h"

namespace tip {

  // Create a FITS file using a template. File is closed afterwards.
  void FitsFileManager::createFile(const std::string & file_name, const std::string & template_name) {
    // Concatenate the file name with the template name:
    std::ostringstream s;
    s << "!" << file_name;
    if (!template_name.empty()) s << "(" << template_name << ")";
    std::string full_name = s.str();

    fitsfile * fp = 0;
    int status = 0;

    // Create the file, and complain if it doesn't work:
    fits_create_file(&fp, const_cast<char *>(full_name.c_str()), &status);

    // Close the file; not interested in it anymore.
    int ignored_status = status;
    fits_close_file(fp, &ignored_status);

    if (0 != status) throw TipException(std::string("Unable to create file named \"") + full_name + '"');
  }

  void FitsFileManager::appendTable(const std::string & file_name, const std::string & table_name) {
    fitsfile * fp = 0;
    int status = 0;

    // Open the file, and complain if it doesn't work:
    fits_open_file(&fp, const_cast<char *>(file_name.c_str()), READWRITE, &status);
    if (0 != status)
      throw TipException(std::string("Unable to open file named \"") + file_name + "\" with read/write access");

    // Create new table extension at end of file.
    fits_create_tbl(fp, BINARY_TBL, 0, 0, 0, 0, 0, const_cast<char *>(table_name.c_str()), &status);

    // Close the file; not interested in it anymore.
    fits_close_file(fp, &status);
  }

}
