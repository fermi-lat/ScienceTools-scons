/** \file FitsFileManager.cxx

    \brief Utilities to help manage FITS files. These classes are not part of the API.

    \author James Peachey, HEASARC
*/

#include <cstdio>
#include <sstream>

#include "FitsFileManager.h"
#include "fitsio.h"
#include "tip/FileSummary.h"
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

  void FitsFileManager::getFileSummary(const std::string & file_name, FileSummary & summary) {
    fitsfile * fp = 0;
    int status = 0;

    // Clear out summary.
    summary.clear();

    // Open the file, and complain if it doesn't work:
    fits_open_file(&fp, const_cast<char *>(file_name.c_str()), READONLY, &status);
    if (0 != status)
      throw TipException(std::string("Unable to open file named \"") + file_name + "\" with read only access");
    
    // Make sure we scan starting from the first extension, regardless of the full file name used.
    fits_movabs_hdu(fp, 1, 0, &status);
    if (0 != status) {
      fits_close_file(fp, &status);
      throw TipException(std::string("Unable to move to primary HDU in file named \"") + file_name);
    }

    // String to hold each extension's identity.
    std::string ext_id;

    while (0 == status) {
      // Get identifier of current extension, and add that info to the summary.
      getExtId(fp, ext_id);
      summary.push_back(ExtSummary(ext_id));

      // Go on to next extension.
      fits_movrel_hdu(fp, 1, 0, &status);
    }
    // Clean up.
    fits_close_file(fp, &status);

    // Flag any condition other than 0 and EOF.
    if (0 != status && END_OF_FILE != status)
      throw TipException(std::string("FitsFileManager::getFileSummary had trouble making summary of file ") + file_name);
  }

  bool FitsFileManager::isValid(const std::string & file_name) {
    fitsfile * fp = 0;
    int status = 0;
    fits_open_file(&fp, const_cast<char *>(file_name.c_str()), READONLY, &status);
    if (0 != status) return false;
    fits_close_file(fp, &status);
    return true;
  }

  void FitsFileManager::getExtId(fitsfile * fp, std::string & ext_id) {
    int status = 0;

    // Make a C array to hold the id string.
    char tmp_id[FLEN_CARD] = "";

    // Try EXTNAME keyword first.
    fits_read_key(fp, TSTRING, "EXTNAME", tmp_id, 0, &status);
    if (0 != status) {
      status = 0;
      // If that didn't work, try HDUNAME.
      fits_read_key(fp, TSTRING, "HDUNAME", tmp_id, 0, &status);
      if (0 != status) {
        // Finally, if that didn't work, just use the number, offset by 1.
        int hdu_num = 0;
        fits_get_hdu_num(fp, &hdu_num);
        sprintf(tmp_id, "%d", hdu_num - 1);
      }
    }
    ext_id = tmp_id;
  }
}
