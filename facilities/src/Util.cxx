// $ Header:$

#include "facilities/Util.h"

/** @file Util.cxx 
    @author J. Bogart
*/

namespace facilities {
  int Util::expandEnvVar(std::string* toExpand,
                         const std::string* openDel,
                         const std::string* closeDel) {

    int nSuccess = 0;

    //now pull out and substitute for environment vaiables
    // This routine, will modify the input string.
    char* op = "$(";
    unsigned opLen = 2;
    char* cl = ")";
    unsigned clLen = 1;
    if (openDel) {
      op = openDel->c_str();
      opLen = openDel->size();
    }

    if (closeDel) {
      cl = closeDel->c_str();
      clLen = closeDel->size();
    }

    int envStart = toExpand->find_first_of(op);
    int envEnd = toExpand->find_first_of(cl);
            
    // add  characters to account for opening delimiter
    int afterBracket = envStart + opLen;
            
    if(!((envStart==-1)||(envEnd==-1)))
    {
      std::string envVariable = 
        toExpand->substr(afterBracket,(envEnd-afterBracket));
      const char * path = ::getenv(envVariable.c_str());
      if(path) {
        toExpand->replace(envStart,(envEnd+clLen), path);
        if (nSuccess > -1) nSuccess++;
      }
      else nSuccess = -1;
    }
    return nSuccess;
  }
}
