#ifndef FluxExecption_H_
#define FluxExecption_H_

/** 
* \class FluxException
*
* \brief Define FATAL MACRO, which throws exception with error message
* \author Toby Burnett
* 
* $Header$
*/

#include <sstream>


#define FATAL_MACRO(output)\
    do{std::stringstream message; \
    message <<__FILE__<<":"<<__LINE__<<": "<<output<<'\0';\
    std::cerr << message.str() << std::endl; \
    throw(message.str());}while(0)

#define WARNING_MACRO(output)\
std::cerr << output << std::endl;

#endif // _ERROR_H_

