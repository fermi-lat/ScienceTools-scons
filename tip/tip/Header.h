/** \file Header.h

    \brief High level encapsulation of a Fits-like header.

    \author James Peachey, HEASARC
*/
#ifndef table_Header_h
#define table_Header_h

#include <string>

#include "table/Header.h"
#include "table/IExtensionData.h"
#include "table/table_types.h"

namespace table {

  /** \class Header

      \brief High level encapsulation of a Fits-like header.
  */
  class Header {
    public:
      Header(IExtensionData * extension_data): m_extension_data(extension_data) {}

      virtual ~Header() {}

      // The following function is only temporary:
      void read(const std::string & name, double & value) { m_extension_data->readKeyword(name, value); }

    private:
      IExtensionData * m_extension_data;
  };

}

#endif
