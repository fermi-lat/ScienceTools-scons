/** \file IFileSvc.h

    \brief Abstract factory for creating, editing and reading data objects.

    \author James Peachey, HEASARC
*/
#ifndef tip_IFileSvc_h
#define tip_IFileSvc_h

#include <string>

#include "tip/FileSummary.h"
#include "tip/Header.h"

namespace tip {

  class Extension;
  class IExtensionData;
  class Image;
  class Table;

  /** \class IFileSvc

      \brief Singleton factory for creating, editing and reading tables and images from files.
  */
  class IFileSvc {
    public:
      /** \brief Singleton access to I/O service objects. Deprecated: used instance() instead!
      */
      static IFileSvc & getSvc();

      /** \brief Singleton access to I/O service objects.
      */
      static IFileSvc & instance();

      /** \brief Perform initializations which are necessary at startup, mainly to handle tweaks to
          Root's global variables.
      */
      static void globalInit();

      /** \brief Destruct an I/O service object.
      */
      virtual ~IFileSvc();

      /** \brief Use a FITS template to create a new file. Clobber existing files.
          \param file_name The name of the new file.
          \param template_name The name of the template file.
      */
      virtual void createFile(const std::string & file_name, const std::string & template_name = "");

      /** \brief Open an existing extension with modification access. The actual object returned
          may be a subclass of Extension, depending on whether the object is a table or image extension.
          \param file_name The name of the file (any supported format OK).
          \param ext_name The name of the extension.
          \param filter Filtering string.
      */
      virtual Extension * editExtension(const std::string & file_name, const std::string & ext_name,
        const std::string & filter = "");

      /** \brief Open an existing image with modification access.
          \param file_name The name of the file (any supported format OK).
          \param table_name The name of the table.
          \param filter Filtering string.
      */
      virtual Image * editImage(const std::string & file_name, const std::string & table_name,
        const std::string & filter = "");

      /** \brief Open an existing table with modification access.
          \param file_name The name of the file (any supported format OK).
          \param table_name The name of the table.
          \param filter Filtering string.
      */
      virtual Table * editTable(const std::string & file_name, const std::string & table_name,
        const std::string & filter = "");

      /** \brief Open an existing extension without modification access. The actual object returned
          may be a subclass of Extension, depending on whether the object is a table or image extension.
          \param file_name The name of the file (any supported format OK).
          \param ext_name The name of the extension.
          \param filter Filtering string.
      */
      virtual const Extension * readExtension(const std::string & file_name, const std::string & ext_name,
        const std::string & filter = "");

      /** \brief Open an existing image without modification access.
          \param file_name The name of the file (any supported format OK).
          \param table_name The name of the table.
          \param filter Filtering string.
      */
      virtual const Image * readImage(const std::string & file_name, const std::string & table_name,
        const std::string & filter = "");

      /** \brief Open an existing table without modification access.
          \param file_name The name of the file (any supported format OK).
          \param table_name The name of the table.
          \param filter Filtering string.
      */
      virtual const Table * readTable(const std::string & file_name, const std::string & table_name,
        const std::string & filter = "");

      /** \brief Obtain summary of the file's contents.
          \param file_name The name of the file.
          \param summary The summary object to fill.
      */
      virtual void getFileSummary(const std::string & file_name, FileSummary & summary);

      /** \brief Check for existence of the named file.
          \param file_name The file name.
      */
      virtual bool fileExists(const std::string & file_name);

      virtual void updateKeywords(const std::string & file_name, const Header::KeyValCont_t & kwds);

    protected:
      /** \brief For singleton pattern, limit creation of IFileSvc objects to derived classes.
      */
      IFileSvc();

      /** \brief Open and return an IExtensionData object for the given file specifier.
          \param file_name The name of the file (any supported format OK).
          \param ext_name The name of the extension.
          \param filter Filtering string.
          \param read_only Should the file be opened read-only.
      */
      virtual IExtensionData * openExtension(const std::string & file_name, const std::string & ext_name,
        const std::string & filter = "", bool read_only = true);
          
    private:
      // Copying file service objects is not supported.
      IFileSvc(const IFileSvc & service) {}
  };

}

#endif
