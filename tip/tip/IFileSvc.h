/** \file IFileSvc.h

    \brief Abstract factory for creating, editing and reading data objects.

    \author James Peachey, HEASARC
*/
#ifndef table_IFileSvc_h
#define table_IFileSvc_h

#include <map>
#include <string>

namespace table {

  class Table;

  /** \class IFileSvc

      \brief Singleton factory for creating, editing and reading tables and images from files.
  */
  class IFileSvc {
    public:
      /** \brief For convenience, typedef FileSvc container.
      */
      typedef std::map<std::string, IFileSvc *> FileSvcCont_t;

      /** \brief Singleton access to I/O service objects.
      */
      static IFileSvc & getSvc();

      /** \brief Destruct an I/O service object.
      */
      virtual ~IFileSvc();

      /** \brief Open an existing table with modification access.
          \param file_name The name of the file (any supported format OK).
          \param table_name The name of the table.
      */
      virtual Table * editTable(const std::string & file_name, const std::string & table_name) = 0;

    protected:
      /** \brief For singleton pattern, limit creation of IFileSvc objects to derived classes.
          \param format_name The name of the file format, set by subclass.
      */
      IFileSvc(const std::string & format_name);

    private:
      // Container of supported file services.
      static FileSvcCont_t m_file_services;
      std::string m_format_name;

      // Copying file service objects is not supported.
      IFileSvc(const IFileSvc & service) {}
  };

}

#endif
