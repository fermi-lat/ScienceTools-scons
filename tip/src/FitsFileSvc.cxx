/** \file FitsFileSvc.cxx

    \brief Implementation of FITS-specific I/O class. This functions as a factory for creating
    data objects which are associated with FITS files.

    \author James Peachey, HEASARC
*/

#include "FitsFileSvc.h"
#include "FitsTabularData.h"
#include "table/Table.h"

namespace table {

  // Create a new I/O class, setting the format_name to be "fits".
  FitsFileSvc::FitsFileSvc(): IFileSvc("fits") {}

  Table * FitsFileSvc::editTable(const std::string & file_name, const std::string & table_name) {
    Table * retval = 0;
    FitsTabularData * data = new FitsTabularData(file_name, table_name);

    if (data) retval = new Table(data);
    return retval;
  }

}
