/** \file FitsFileSvc.cxx

    \brief Factory for handling Fits objects.

    \author James Peachey, HEASARC
*/

#include "FitsFileSvc.h"
#include "FitsTabularData.h"
#include "table/Table.h"

namespace table {

  FitsFileSvc::FitsFileSvc(const std::string & name): IFileSvc(name) {}

  Table * FitsFileSvc::editTable(const std::string & file_name, const std::string & table_name) {
    Table * retval = 0;
    FitsTabularData * data = new FitsTabularData(file_name, table_name);

    if (data) retval = new Table(data);
    return retval;
  }

}
