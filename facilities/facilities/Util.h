// $ Header:$
#ifndef FACILITIES_UTIL_H
#define FACILITIES_UTIL_H

#include <string>

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
    static int         atoi(const std::string& InStr);
  };
}

#endif
