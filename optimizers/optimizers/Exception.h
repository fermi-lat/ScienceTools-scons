/**
 * @file Exception.h
 * @brief Exception class for optimizers
 * @author P. Nolan
 *
 * $Header$
 */

#ifndef optimizers_EXCEPTION_H
#define optimizers_EXCEPTION_H
#include <exception>
#include <string>

namespace optimizers {
/**
 * @class Exception
 *
 * @brief Exception class for optimizers
 *
 * @author P. Nolan
 *
 * $Header$
 */

  class Exception: public std::exception {
  public:
    Exception() {}
    Exception(std::string errorString, int code=0) : 
      m_what(errorString), m_code(code) 
      {}
    virtual ~Exception() throw() {}
    virtual const char *what() const throw() {return m_what.c_str();}
    virtual const int code() const {return m_code;}
  protected:
    std::string m_what;
    int m_code;
  };
}
#endif //optimizers_EXCEPTION_H
