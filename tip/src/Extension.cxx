/** \file Extension.cxx

    \brief High level encapsulation of a FITS-like file extension, which may or may not actually BE in FITS format.

    \author James Peachey, HEASARC
*/

#include "tip/Extension.h"
#include "tip/IExtensionManager.h"
#include "tip/Header.h"
#include "tip/TipException.h"
#include "tip/tip_types.h"

namespace tip {

  Extension::Extension(IExtensionManager * extension_manager): m_header(0), m_extension_manager(extension_manager) {
    if (m_extension_manager) m_header = new Header(m_extension_manager->getHeaderData());
    else throw TipException("Extension::Extension(IExtensionManager *): Cannot create Extension object "
      "with NULL IExtensionManager pointer.");
  }

  Extension::~Extension() { delete m_header; delete m_extension_manager; }

  Header & Extension::getHeader() { return *m_header; }

}
