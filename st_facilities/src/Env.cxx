/**
 * @file Env.h
 * @brief Implementation of Env class
 * @authors James Peachey, HEASARC/GSSC
 *
 * $Header$
 *
 */
#include <iostream>

#include <cctype>
#include <cstdlib>
#include <list>
#include <stdexcept>

#include "st_facilities/Env.h"

#ifdef WIN32
static const std::string sFileDelim= "\\";
static const std::string sPathDelim = ";";
#else
static const std::string sFileDelim= "/";
static const std::string sPathDelim = ":";
#endif

namespace st_facilities {

  std::string Env::appendFileName(const std::string & dir, const std::string & file) {
    return join(dir, file, sFileDelim);
  }

  std::string Env::appendPath(const std::string & path, const std::string & dir) {
    return join(path, dir, sPathDelim);
  }

  void Env::expandEnvVar(const std::string & to_expand, std::string & expanded) {
    bool expansion_failed = false;
    // Work with a local copy of the output string so that input and output may in fact be the same string.
    std::string output;
    output.reserve(to_expand.size());

    // Make one pass through the input string.
    for (std::string::const_iterator cur_pos = to_expand.begin(); cur_pos != to_expand.end(); ++cur_pos) {
      // A $ heralds the beginning of what may be an environment variable name.
      if ('$' == *cur_pos) {
        std::string::const_iterator begin_name = cur_pos + 1;
        std::string::const_iterator end_name = begin_name;

        // First pattern: see if name contains alphanumeric or _ only.
        while (end_name != to_expand.end() && (0 != isalnum(*end_name) || '_' == *end_name)) ++end_name;

        // Store result in local name variable.
        std::string name(begin_name, end_name);

        // See if name contains anything; this indicates a match occurred.
        if (name.empty()) {
          // No match to the first pattern.
          // Try second pattern: { or ( followed by some characters, followed by ) or }.
          if ('{' == *begin_name || '(' == *begin_name) {
            // Set appropriate terminating character to match: {} or ()
            char term = ('{' == *end_name) ? '}' : ')';

            // Skip past { or (: they are not part of the name, although they are part of the sequence
            // containing the name.
            end_name = ++begin_name;

            // Look from here onward until the terminating character is found.
            for (; end_name != to_expand.end(); ++end_name) {
              if (term == *end_name) {
                // Termination was found, so assign the name.
                name.assign(begin_name, end_name);

                // Increment end marker. This is necessary to keep end_name consistent with the first pattern.
                // End_name needs to point one position past the end of the name sequence.
                ++end_name;
                break;
              }
            }
          }

          // If still no match, this is an error, because $ appeared without a valid name following it.
          if (name.empty()) {
            expanded = to_expand;
            throw std::runtime_error("Env::expandEnvVar failed to parse string \"" + to_expand + "\"");
          }
        }

        try {
          // Attempt to expand the variable and add it to the output.
          output += getEnv(name);

          // The name was consumed by the expansion, so continue iterating after last character in the name. Note that
          // cur_pos will be incremented at the top of the loop, so 1 must be subtracted so no characters are skipped.
          cur_pos = end_name - 1;
          continue;
        } catch (const std::exception &) {
          // Note the problem, but do not let it arrest expansions in case the client wishes to ignore the problem.
          expansion_failed = true;
        }
      }

      // Fell through, so either there was never a possibility of an env variable name, or expansion failed.
      output += *cur_pos;
    }

    // Copy local output to the output parameter.
    expanded = output;

    if (expansion_failed) {
      throw std::runtime_error("st_facilities::Env::expandEnvVar failed to expand one or more environment "
        "variables in string \"" + to_expand + "\"");
    }
  }

  std::string Env::getEnv(const std::string & name) {
    std::string retval;
    const char * cp = ::getenv(name.c_str());
    if (0 != cp) retval = cp;
    else throw std::runtime_error("Env::getEnv could not expand the name \"" + name + "\"");
    return retval;
  }

  std::string Env::getDataDir(const std::string & pkg_id) {
    std::string dir;

    // First attempt to expand the pattern $<pkg>ROOT/data.
    try {
      expandEnvVar(appendFileName(getPkgRoot(pkg_id), "data"), dir);
    } catch (const std::exception &) {
      // Expansion failed, so try the install area.
      dir = getEnv("DATAPATH");
    }

    return dir;
  }

  std::string Env::getXmlDir(const std::string & pkg_id) {
    std::string dir;

    // First attempt to expand the pattern $<pkg>ROOT/data.
    try {
      expandEnvVar(appendFileName(getPkgRoot(pkg_id), "xmlData"), dir);
    } catch (const std::exception &) {
      // Expansion failed, so try the install area.
      dir = getEnv("XMLPATH");
    }

    return dir;
  }

  std::string Env::getPkgRoot(const std::string & pkg_id) {
    if (pkg_id.empty()) throw std::runtime_error("Env::getPkgRoot was passed a blank package identifier.");
    std::string retval("$" + pkg_id + "ROOT");
    for (std::string::iterator itor = retval.begin(); itor != retval.end(); ++itor) *itor = toupper(*itor);
    return retval;
  }

  std::string Env::join(const std::string & string1, const std::string & string2, const std::string delim) {
    std::string retval;

    if (string1.empty()) retval = string2;
    else if (string2.empty()) retval = string1;
    else retval = string1 + delim + string2;

    return retval;
  }

} // namespace st_facilities
