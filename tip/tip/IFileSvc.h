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
      typedef std::map<std::string, IFileSvc *> FileSvcCont_t;

      static IFileSvc & getSvc();

      virtual ~IFileSvc();

      /** \brief Open an existing table with modification access.
      */
      virtual Table * editTable(const std::string & file_name, const std::string & table_name) = 0;

    protected:
      IFileSvc(const std::string & name);

    private:
      static FileSvcCont_t m_file_services;
      std::string m_name;

      // Copying file service objects is not supported.
      IFileSvc(const IFileSvc & service) {}
  };

}

#endif
