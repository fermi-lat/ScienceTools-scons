/** \file Extension.h

    \brief High level encapsulation of a FITS-like file extension, which may or may not actually BE in FITS format.

    \author James Peachey, HEASARC
*/
#ifndef table_Extension_h
#define table_Extension_h

#include <string>

#include "table/IExtensionManager.h"
#include "table/Header.h"
#include "table/TableException.h"
#include "table/table_types.h"

namespace table {

  /** \class Extension

      \brief High level encapsulation of a FITS-like file extension.
  */
  class Extension {
    public:
      /** \brief Create a new extension object from the given abstract extension data encapsulation.
          \param extension_data The extension data (concrete instances are FITS or Root specific).
      */
      Extension(IExtensionManager * extension_data): m_header(0), m_extension_data(extension_data) {
        if (m_extension_data) m_header = new Header(m_extension_data->getHeaderData());
        else throw TableException("Extension::Extension(IExtensionManager *): Cannot create Extension object "
          "with NULL IExtensionManager pointer.");
      }

      /** \brief Destruct an extension object.
      */
      virtual ~Extension() { delete m_header; delete m_extension_data; }

      /** \brief Retrieve Header object, which is a container of FITS-like keywords.
      */
      Header & getHeader() {
        return *m_header;
      }

    private:
      Header * m_header;
      IExtensionManager * m_extension_data;
  };

}

#endif
