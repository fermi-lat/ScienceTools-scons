// $ Header:$

#include "facilities/Util.h"

/** @file Util.cxx 
    @author J. Bogart
*/

namespace facilities {

  int Util::expandEnvVar(std::string* toExpand, 
                         const std::string& openDel,
                         const std::string& closeDel) {
    unsigned opLen = openDel.size();
    unsigned clLen = closeDel.size();
    int nSuccess = 0;
                           
    int envStart = toExpand->find_first_of(openDel.c_str());
    while (envStart != -1) {
      int envEnd = toExpand->find_first_of(closeDel.c_str());
            
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
      envStart = toExpand->find_first_of(openDel.c_str());
    }
    return nSuccess;
  }

}
