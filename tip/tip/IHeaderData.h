/** \file IHeaderData.h

    \brief Low level abstract interface to header data.

    \author James Peachey, HEASARC
*/
#ifndef table_IHeaderData_h
#define table_IHeaderData_h

#include <string>

#include "table/table_types.h"

namespace table {

  /** \class IHeaderData

      \brief Low level abstract interface to header data.
  */
  class IHeaderData {
    public:
      virtual ~IHeaderData() {}

      /** \brief Read a keyword from the current header data object.
          \param field The name of the field (column) to read.
          \param value The output value.
      */
      virtual void read(const std::string & field, double & value) const = 0;
  };

}

#endif
