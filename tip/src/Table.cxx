/** \file Table.cxx

    \brief Low level implementation for common functionality of any supported Table type.

    \author James Peachey, HEASARC
*/

#include "table/Table.h"
#include "FitsTable.h"

namespace table {

  Table * Table::openReadWrite(const std::string & file_name, const std::string & table_name) {
    Table * retval = 0;

    // For now, just try to open a FITS table.
    retval = new FitsTable(file_name, table_name);

    return retval;
  }

}
