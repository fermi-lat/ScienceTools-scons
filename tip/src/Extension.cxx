/** \file Extension.cxx

    \brief High level encapsulation of a FITS-like file extension, which may or may not actually BE in FITS format.

    \author James Peachey, HEASARC
*/

#include "table/Extension.h"
#include "table/IExtensionManager.h"
#include "table/Header.h"
#include "table/TableException.h"
#include "table/table_types.h"

namespace table {

  Extension::Extension(IExtensionManager * extension_manager): m_header(0), m_extension_manager(extension_manager) {
    if (m_extension_manager) m_header = new Header(m_extension_manager->getHeaderData());
    else throw TableException("Extension::Extension(IExtensionManager *): Cannot create Extension object "
      "with NULL IExtensionManager pointer.");
  }

  Extension::~Extension() { delete m_header; delete m_extension_manager; }

  Header & Extension::getHeader() { return *m_header; }

}
