/**
 * @file FileSys.h
 * @brief Declaration of FileSys class
 * @authors James Peachey, HEASARC/GSSC
 *
 * $Header$
 *
 */

#ifndef st_facilities_FileSys_h
#define st_facilities_FileSys_h

#include <string>
#include <vector>

namespace st_facilities {

/**
 * @class FileSys
 *
 * @brief A class which handles various low-level interactions with the file system.
 *
 * @author James Peachey, HEASARC/GSSC
 *
 * $Header$
 *
 */

class FileSys {

public:

   typedef std::vector<std::string> FileNameCont;

   /** @brief Read a file which contains a list of files, and return the list.

              If the input file string starts with @, the string following the @ will be used as the name of the
              file containing the list. The file is assumed to contain a list of files, one per line.
              If the string does not begin with @, the string is assumed to contain just the name of a single
              file. This file name (i.e. the input file name) will be returned in the container. In either case,
              environment variable names will be expanded, both for the input file name and for its contents.
       @param file The name of the file being expanded.
   */
   static FileNameCont expandFileList(const std::string & file);

};

} // namespace st_facilities

#endif
