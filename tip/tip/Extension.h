/** \file Extension.h

    \brief High level encapsulation of a FITS-like file extension, which may or may not actually BE in FITS format.

    \author James Peachey, HEASARC
*/
#ifndef table_Extension_h
#define table_Extension_h

#include <string>

#include "table/Header.h"
#include "table/table_types.h"

namespace table {

  // Abstract interface which is subclassed for actual file formats (e.g. FITS, Root, etc.)
  class IExtensionData;

  /** \class Extension

      \brief High level encapsulation of a FITS-like file extension.
  */
  class Extension {
    public:
      /** \brief Create a new extension object from the given abstract extension data encapsulation.
          \param extension_data The extension data (concrete instances are FITS or Root specific).
      */
      Extension(IExtensionData * extension_data): m_header(extension_data) {}

      /** \brief Destruct an extension object.
      */
      virtual ~Extension() {}

      /** \brief Retrieve Header object, which is a container of FITS-like keywords.
      */
      Header & getHeader() { return m_header; }

    private:
      Header m_header;
  };

}

#endif
