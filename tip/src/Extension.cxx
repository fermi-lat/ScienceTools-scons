/** \file Extension.cxx

    \brief High level encapsulation of a FITS-like file extension, which may or may not actually BE in FITS format.

    \author James Peachey, HEASARC
*/

#include "tip/Extension.h"
#include "tip/Header.h"
#include "tip/TipException.h"

namespace tip {

  Extension::Extension(IExtensionData * extension_data): m_header(extension_data), m_extension_data(extension_data) {
    if (0 == m_extension_data) throw TipException("Extension::Extension(IExtensionData *): Cannot create Extension object "
        "with NULL IExtensionData pointer.");
  }

  Extension::~Extension() { delete m_extension_data; }

  Header & Extension::getHeader() { return m_header; }

  const Header & Extension::getHeader() const { return m_header; }

}
