/** \file IExtensionData.h

    \brief Abstract interface to a FITS-like extension, containing a header and some sort of data (table or image).

    \author James Peachey, HEASARC
*/
#ifndef table_IExtensionData_h
#define table_IExtensionData_h

#include <string>

namespace table {

  class IHeaderData;
  class ITabularData;

  /** \class IExtensionData

      \brief Abstract interface to a FITS-like extension, containing a header and some sort of data (table or image).
  */
  class IExtensionData {
    public:
      virtual ~IExtensionData() {}

      /** \brief Return an interface for accessing this extensions header properties (i.e. keywords).
      */
      virtual IHeaderData * getHeaderData() = 0;

      /** \brief Return an interface for accessing this extensions table properties. If the underlying
          object does not contain a table, the implmementation for this method must return a NULL pointer.
      */
      virtual ITabularData * getTabularData() = 0;
  };

}

#endif
