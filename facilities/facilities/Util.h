// $ Header:$
#ifndef FACILITIES_UTIL_H
#define FACILITIES_UTIL_H

#include <string>
#include <vector>

/** @file Util.h
    @author J. Bogart

    This file declares the class Util for basic static utilities 
    and an associated exception class.
    
*/
namespace facilities {
  /// This class provides a home for utility functions with no need for
  /// any context (hence static)
  class Untranslatable {
  public:
    Untranslatable(const std::string& toTrans) : badVar(toTrans) {}
    std::string badVar;
  };

  class Util {
  public:
    /** Given input string @a toExpand expand references to environment
        variables, by default of the form $(varname) and put the 
        expanded version back into the original string.  Alternate
        delimiters for the @a varname may optionally be specified
        @param   toExpand string for which expansion is to be done
        @param   openDel opening delimiter (defaults to "$(")
        @param   closeDel closing delimiter (defaults to ")")
        
        @return  -1 if attempt at expansion failed at least once,
                 else number of successful expansions.

        TODO:  Perhaps add optional arguments to specify alternate
               delimiters.
    */
    static int expandEnvVar(std::string* toExpand, 
                            const std::string& openDel = std::string("$("),
                            const std::string& closeDel = std::string(")"));

    /** Given an input integer @a val to convert and an output string @a outStr
      converts val into a std::string.
      This method duplicates the stdlib.h method itoa, except that it returns
      std::string rather than char*.
      @param   val
      @param   outStr will be modified by this method

      @return  const char* based on the contents of outStr.c_str()
    */
    static const char* itoa(int val, std::string &outStr);

    /// converts an std::string to an integer
    static int atoi(const std::string& InStr);

    /** This routine breaksdown a string into tokens, based on the
        characters appearing in the string @a delimiters.
        @param input       string to be tokenized
        @param delimiters  string containing one or more delimiter characters
        @param tokens      vector of strings to hold resulting tokens
        @param clear       if true (default) @a tokens will be cleared
                           at the start of processing
    */
    static void stringTokenize(std::string input, const std::string &delimiters,
                               std::vector<std::string> &tokens,
                               bool clear = true);

    /** return the "non-directory" part of a (supposed) file identifier, @a path.
        Environment variable translation should be done before calling @a basename.
        @sa { Util::expandEnvVar }
        @param path        string assumed to be a file identifier. 
    */
    static std::string basename(const std::string &path);
  };
}

#endif
