/** \file FitsFileManager.h

    \brief Utilities to help manage FITS files. These classes are not part of the API.

    \author James Peachey, HEASARC
*/
#ifndef tip_FitsFileManager_h
#define tip_FitsFileManager_h

#include <string>

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
  };

}

#endif
