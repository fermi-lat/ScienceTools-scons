/**
 * @file Env.h
 * @brief Declaration of Env class
 * @authors James Peachey, HEASARC/GSSC
 *
 * $Header$
 *
 */

#ifndef st_facilities_Env_h
#define st_facilities_Env_h

#include <string>

namespace st_facilities {

/**
 * @class Env
 *
 * @brief A class for determining runtime properties based on the environment. It takes into account differences
 *        between operating systems.
 *
 * @author James Peachey, HEASARC/GSSC
 *
 * $Header$
 *
 */

class Env {

public:

   /** @brief Append a file name to a directory name. This is done in an OS-appropriate way, i.e. delimited by
              slashes on Unix and backslashes on Windows. Neither the directory nor the file name being added is checked
              for validity. The fully qualified file name is returned.
       @param dir The original directory. May be blank.
       @param file The file being added. May be blank.
   */
   static std::string appendFileName(const std::string & dir, const std::string & file);

   /** @brief Append a directory name to a path. This is done in the OS-appropriate way, i.e. delimited by
              colons on Unix and semicolons on Windows. Neither the path nor the part being added is checked
              for validity. The new path is returned.
       @param path The original path. May be blank.
       @param dir The directory being added. May be blank.
   */
   static std::string appendPath(const std::string & path, const std::string & dir);

   /** @brief Expand all environment variables in the input string and return in the output string.
              Environment variable names may be specified in one of three forms: $sequence, ${sequence}
              or $(sequence), where sequence is any sequence of letters, numbers or underscores.
              Any variables which are not set in the environment will simply not be expanded. In
              this case, an exception is thrown, but only after the output string has been expanded as
              much as possible. This makes it simple for clients to ignore the exception.
       @param to_expand The input string to be expanded.
       @param expanded The expanded string. May be the same as the input string.
   */
   static void expandEnvVar(const std::string & to_expand, std::string & expanded);

   /** @brief Expand the single environment variable given in the name parameter. Legal characters in the
              name include alphanumerics and underscore only. If the environment variable is not set, an
              exception is thrown. This is basically a C++-idiomatic front-end to getenv.
       @param name The name of the environment variable.
   */
   static std::string getEnv(const std::string & name);

   /** @brief Given the package root name, return the name of the directory which contains its ancillary data files.
       @param pkg_id The name of the package. May be blank.
   */
   static std::string getDataDir(const std::string & pkg_id);

   /** @brief Given the package root name, return the name of the directory which contains its ancillary XML files.
       @param pkg_id The name of the package. May be blank.
   */
   static std::string getXmlDir(const std::string & pkg_id);

private:

   /** @brief Given the package root name, return the full name of the environment variable connected with that
              package in the build area. Throws if the pkg_id is blank.
       @param pkg_id The name of the package.
   */
   static std::string getPkgRoot(const std::string & pkg_id);

   /** @brief Utility to join two strings, using a delimiter between them. Any/all of the three arguments
              may be blank.
       @param string1 The first string.
       @param string2 The second string.
       @param delim The delimiter with which to connect the strings.
   */
   static std::string join(const std::string & string1, const std::string & string2, const std::string delim);

};

} // namespace st_facilities

#endif
