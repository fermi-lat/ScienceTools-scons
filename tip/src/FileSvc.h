/** \file FitsFileSvc.h

    \brief FITS-specific I/O class. This functions as a factory for creating
    data objects which are associated with FITS files.

    \author James Peachey, HEASARC
*/
#ifndef table_FitsFileSvc_h
#define table_FitsFileSvc_h

#include "table/IFileSvc.h"

namespace table {

  /** \class FitsFileSvc

      \brief Singleton factory for creating, editing and reading tables and images from files.
  */
  class FitsFileSvc : public IFileSvc {
    public:

      /** \brief Create a new I/O accessor for FITS files.
      */
      FitsFileSvc();

      /** \brief Open an existing FITS table with modification access.
          \param file_name The name of the FITS file.
          \param table_name The name of the FITS table.
      */
      virtual Table * editTable(const std::string & file_name, const std::string & table_name);
  };

}

#endif
