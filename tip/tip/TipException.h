/** \file TableException.h

    \brief Exceptions thrown by table objects.

    \author James Peachey, HEASARC
*/
#ifndef table_TableException_h
#define table_TableException_h

#include <exception>
#include <string>

namespace table {

  /** \class TableException

      \brief Base class for exceptions thrown by table objects.
  */
  class TableException : public std::exception {
    public:
      TableException(const std::string & msg = "Table component exception"): std::exception(), m_msg(msg) {}
      virtual ~TableException() throw() {}
      virtual const char * what() const throw() { return m_msg.c_str(); }

    private:
      std::string m_msg;
  };

}

#endif
