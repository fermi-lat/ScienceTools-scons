/**
 * @file FileSys.cxx
 * @brief Implementation of FileSys class
 * @authors James Peachey, HEASARC/GSSC
 *
 * $Header$
 *
 */

#include <cstdio>
#include <fstream>
#include <stdexcept>

#include "st_facilities/Env.h"
#include "st_facilities/FileSys.h"

namespace st_facilities {

  FileSys::FileNameCont FileSys::expandFileList(const std::string & file) {
    // Expand the original file name string.
    std::string expanded_file;
    Env::expandEnvVar(file, expanded_file);

    // Create container to return.
    FileNameCont cont(0, "");
    if (expanded_file[0] == '@') {
      std::string expanded_buf;
      char buf[FILENAME_MAX + 1]; // Buffer to hold lines from input file.

      // Open the "at" file: contains a list of files.
      std::ifstream ifile(expanded_file.c_str() + 1);
      if (!ifile) throw std::runtime_error("FileSys::expandFileList could not open file " + expanded_file);

      // Iterate over file list.
      do {
        *buf = '\0';
        ifile.getline(buf, FILENAME_MAX);
        if ('\0' != *buf) {
          // Line contains some text, so expand environment variables.
          expanded_buf.erase();
          Env::expandEnvVar(buf, expanded_buf);

          // Place expanded name in output.
          cont.push_back(expanded_buf);
        }
      } while (ifile);
    } else {
      // File does not contain a list of files; just return expanded file name.
      cont.push_back(expanded_file);
    }
    return cont;
  }

} // namespace st_facilities
