/** \file IExtensionManager.h

    \brief Abstract interface to a FITS-like extension, containing a header and some sort of data (table or image).

    \author James Peachey, HEASARC
*/
#ifndef tip_IExtensionManager_h
#define tip_IExtensionManager_h

namespace tip {

  class IHeaderData;
  class ITabularData;

  /** \class IExtensionManager

      \brief Abstract interface to a FITS-like extension, containing a header and some sort of data (table or image).
  */
  class IExtensionManager {
    public:
      virtual ~IExtensionManager() {}

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
