/** \file Extension.h

    \brief High level encapsulation of a FITS-like file extension, which may or may not actually BE in FITS format.

    \author James Peachey, HEASARC
*/
#ifndef table_Extension_h
#define table_Extension_h

namespace table {

  class Header;
  class IExtensionManager;

  /** \class Extension

      \brief High level encapsulation of a FITS-like file extension.
  */
  class Extension {
    public:
      /** \brief Create a new extension object from the given abstract extension manager encapsulation.
          \param extension_manager The extension manager, which creates and manages the actual implementation,
          including file format-specific details.
      */
      Extension(IExtensionManager * extension_manager);

      /** \brief Destruct an extension object.
      */
      virtual ~Extension();

      /** \brief Retrieve Header object, which is a container of FITS-like keywords.
      */
      Header & getHeader();

    private:
      Header * m_header;
      IExtensionManager * m_extension_manager;
  };

}

#endif
