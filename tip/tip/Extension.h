/** \file Extension.h

    \brief High level encapsulation of a FITS-like file extension, which may or may not actually BE in FITS format.

    \author James Peachey, HEASARC
*/
#ifndef tip_Extension_h
#define tip_Extension_h

#include "tip/Header.h"

namespace tip {

  class IExtensionData;

  /** \class Extension

      \brief High level encapsulation of a FITS-like file extension.
  */
  class Extension {
    public:
      /** \brief Create a new extension object from the given abstract extension data encapsulation.
          \param extension_data The extension data, which creates and manages the actual implementation,
          including file format-specific details.
      */
      Extension(IExtensionData * extension_data);

      /** \brief Destruct an extension object.
      */
      virtual ~Extension();

      /** \brief Retrieve Header object, which is a container of FITS-like keywords.
      */
      Header & getHeader();

    private:
      Header m_header;
      IExtensionData * m_extension_data;
  };

}

#endif
