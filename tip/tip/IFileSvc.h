/** \file IFileSvc.h

    \brief Abstract factory for creating, editing and reading data objects.

    \author James Peachey, HEASARC
*/
#ifndef tip_IFileSvc_h
#define tip_IFileSvc_h

#include <string>

namespace tip {

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

      /** \brief Open an existing table with modification access.
          \param file_name The name of the file (any supported format OK).
          \param table_name The name of the table.
          \param filter Filtering string.
      */
      virtual Table * editTable(const std::string & file_name, const std::string & table_name,
        const std::string & filter = "");

      /** \brief Open an existing table without modification access.
          \param file_name The name of the file (any supported format OK).
          \param table_name The name of the table.
          \param filter Filtering string.
      */
      virtual const Table * readTable(const std::string & file_name, const std::string & table_name,
        const std::string & filter = "");

    protected:
      /** \brief For singleton pattern, limit creation of IFileSvc objects to derived classes.
      */
      IFileSvc();

    private:
      // Copying file service objects is not supported.
      IFileSvc(const IFileSvc & service) {}
  };

}

#endif
