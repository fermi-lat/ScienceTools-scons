/** \file IFileSvc.cxx

    \brief Factory for handling table objects.

    \author James Peachey, HEASARC
*/

#include "FitsExtensionManager.h"
#include "RootExtensionManager.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "tip/TipException.h"

namespace tip {

  // Deprecated.
  IFileSvc & IFileSvc::getSvc() {
    return instance();
  }

  // Get the instance of thie factory.
  IFileSvc & IFileSvc::instance() {
    static bool first_time = true;
    if (first_time) {
      // Make sure global initialization has been performed:
      globalInit();
    }

    // Create the singleton factory.
    static IFileSvc s_file_factory;

    return s_file_factory;
  }

  // Perform global initializations.
  void IFileSvc::globalInit() {
    RootExtensionManager::resetSigHandlers();
  }

  // Destructor for a file service.
  IFileSvc::~IFileSvc() {
  }

  // Edit a table in a file, be it FITS or Root.
  Table * IFileSvc::editTable(const std::string & file_name, const std::string & table_name) {
    Table * retval = 0;
    IExtensionManager * data = 0;
    try {
      try {
        data = new FitsExtensionManager(file_name, table_name);
      } catch(TipException & x) {
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

  // Protected constructor which adds the current object to the registry of IFileSvc objects.
  IFileSvc::IFileSvc() {}

}
