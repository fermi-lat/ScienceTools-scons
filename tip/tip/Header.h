/** \file Header.h

    \brief High level encapsulation of a Fits-like header.

    \author James Peachey, HEASARC
*/
#ifndef table_Header_h
#define table_Header_h

#include <string>

#include "table/Header.h"
#include "table/table_types.h"

namespace table {

  class IHeaderData;

  /** \class Header

      \brief High level encapsulation of a Fits-like header.
  */
  class Header {
    public:

      Header(IHeaderData * header_data): m_header_data(header_data) {}

      virtual ~Header() { /* delete m_header_data; */ }

    private:
      IHeaderData * m_header_data;
  };

}

#endif
