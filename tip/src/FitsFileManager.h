/** \file FitsFileManager.h

    \brief Utilities to help manage FITS files. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef tip_FitsFileManager_h
#define tip_FitsFileManager_h

#include <string>
#include "fitsio.h"
#include "tip/FileSummary.h"

namespace tip {
  /** \class FitsFileManager
      \brief Low level interface to FITS files. This is not part of the API. This class is concerned
      with whole files, as distinct from other classes which handle individual file extensions.
  */
  class FitsFileManager {
    public:
      /** \brief Create a new file, using an optional FITS template file. Clobber existing files.
          \param file_name The name of the new file.
          \param template_name The name of the template file. If omitted, the file created will
          be pretty useless.
      */
      static void createFile(const std::string & file_name, const std::string & template_name = "");

      /** \brief Append a table extension to a file.
          \param file_name The name of the file to which to append.
          \param table_name The name of the new table.
      */
      static void appendTable(const std::string & file_name, const std::string & table_name);

      /** \brief Obtain summary of contents of the given file.
          \param file_name The name of the file.
          \param summary The object holding the summary.
      */
      static void getFileSummary(const std::string & file_name, FileSummary & summary);

      /** \brief Determine if the given file name is the name of a FITS file.
          \param file_name The name of the file.
      */
      static bool isValid(const std::string & file_name);

    private:
      // Get the extsnsion identifier (name or number).
      static void getExtId(fitsfile * fp, std::string & ext_id);
  };

}

#endif
