/** \file FileSvc.h

    \brief Declaration of I/O singleton factory class. This functions as a factory for creating
    data objects which are associated with FITS or Root files.

    \author James Peachey, HEASARC
*/
#ifndef table_FileSvc_h
#define table_FileSvc_h

#include "table/IFileSvc.h"

namespace table {

  /** \class FileSvc

      \brief Singleton factory for creating, editing and reading tables and images from files.
  */
  class FileSvc : public IFileSvc {
    public:

      /** \brief Create a new I/O accessor for files.
      */
      FileSvc();

      /** \brief Open an existing table with modification access.
          \param file_name The name of the file containing the table.
          \param table_name The name of the table.
      */
      virtual Table * editTable(const std::string & file_name, const std::string & table_name);
  };

}

#endif
