/** \file Table.cxx

    \brief Low level implementation for common functionality of any supported Table type.

    \author James Peachey, HEASARC
*/

#include "table/Table.h"
#include "FitsTabularData.h"

namespace table {

  Table * Table::openReadWrite(const std::string & file_name, const std::string & table_name) {
    Table * retval = 0;

    // For now, just try to open a FITS table.
    FitsTabularData * data = new FitsTabularData(file_name, table_name);

    if (data) retval = new Table(data);

    return retval;
  }

}
