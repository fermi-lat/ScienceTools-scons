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
      Note that despite the FITS-specific name, this will (for now at least) also try to open
      Root files! This class should thus be renamed eventually, or else a Root-specific FileSvc
      developed
  */
  class FitsFileSvc : public IFileSvc {
    public:

      /** \brief Create a new I/O accessor for FITS files.
      */
      FitsFileSvc();

      /** \brief Open an existing FITS table or Root table (for now) with modification access.
          \param file_name The name of the FITS file.
          \param table_name The name of the FITS table.
      */
      virtual Table * editTable(const std::string & file_name, const std::string & table_name);
  };

}

#endif
