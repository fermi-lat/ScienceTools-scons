/** \file TableException.h

    \brief Exceptions thrown by table objects.

    \author James Peachey, HEASARC
*/
#ifndef table_TableException_h
#define table_TableException_h

#include <exception>

namespace table {

  /** \class TableException

      \brief Base class for exceptions thrown by table objects.
  */
  class TableException : public std::exception {
    public:
      virtual const char * what() const throw() { return "Table component exception"; }
  };

}

#endif
