/** \file FileSvc.cxx

    \brief Implementation of I/O singleton factory class. This functions as a factory for creating
    data objects which are associated with FITS or Root files.

    \author James Peachey, HEASARC
*/
#include "FileSvc.h"
#include "FitsExtensionManager.h"
#include "RootExtensionManager.h"
#include "table/Table.h"

namespace table {

  // Create a new I/O class, setting the format_name to be "default".
  // Note that despite the name, this will (for now at least) also
  // try to open Root files! This class should thus be renamed eventually,
  // or else a different division of labor adopted.
  FileSvc::FileSvc(): IFileSvc("default") {}

  Table * FileSvc::editTable(const std::string & file_name, const std::string & table_name) {
    Table * retval = 0;
    IExtensionManager * data = 0;
    try {
      try {
        data = new FitsExtensionManager(file_name, table_name);
      } catch(TableException & x) {
        data = new RootExtensionManager(file_name, table_name);
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
