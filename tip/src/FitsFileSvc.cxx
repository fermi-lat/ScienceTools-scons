/** \file FitsFileSvc.cxx

    \brief Implementation of FITS-specific I/O class. This functions as a factory for creating
    data objects which are associated with FITS files.

    \author James Peachey, HEASARC
*/
#include "FitsExtensionData.h"
#include "FitsFileSvc.h"
#include "RootExtensionData.h"
#include "table/Table.h"

namespace table {

  // Create a new I/O class, setting the format_name to be "fits".
  // Note that despite the name, this will (for now at least) also
  // try to open Root files! This class should thus be renamed eventually,
  // or else a different division of labor adopted.
  FitsFileSvc::FitsFileSvc(): IFileSvc("fits") {}

  Table * FitsFileSvc::editTable(const std::string & file_name, const std::string & table_name) {
    Table * retval = 0;
    IExtensionData * data = 0;
    try {
      try {
        data = new FitsExtensionData(file_name, table_name);
      } catch(TableException & x) {
        data = new RootExtensionData(file_name, table_name);
      }
      retval = new Table(data);
    } catch(...) {
      if (retval) delete retval; // If retval is non-0, Table was created, so it will delete data.
      else delete data; // Thus, don't delete it twice.
      throw;
    }
    return retval;
  }

}
