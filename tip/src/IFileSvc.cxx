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

  // Create a file, for now FITS only.
  void IFileSvc::createFile(const std::string & file_name, const std::string & template_name) {
    FitsExtensionManager::createFile(file_name, template_name);
  }

  // Edit a table in a file, be it FITS or Root.
  Table * IFileSvc::editTable(const std::string & file_name, const std::string & table_name,
    const std::string & filter) {
    Table * retval = 0;
    IExtensionManager * data = 0;
    TipException fits_exception;
    try {
      try {
        data = new FitsExtensionManager(file_name, table_name, filter);
      } catch(const TipException & x) {
        fits_exception = x;
        data = new RootExtensionManager(file_name, table_name, filter);
      }
      retval = new Table(data);
    } catch(const TipException & x) {
      delete retval; // If retval is non-0, Table was created, so it will delete data.
      throw TipException(std::string(fits_exception.what()) + "\n" + x.what());
    } catch(...) {
      delete retval; // If retval is non-0, Table was created, so it will delete data.

      /* TODO 1: 4/2/2004: Memory management problem: Extension is base of Table. Extension
      has a IExtensionManager and ~Extension deletes it. Currently editTable creates
      the IExtensionManager and passes it to Table::Table(...) which passes it to
      Extension::Extension(...). If something throws along the way, catch 22: If
      Extension throws, editTable should delete the IExtensionManager because
      ~Extension wont be called. If Table throws, editTable shouldn't delete it
      because ~Extension *will* be called. FOR NOW: take out editTable's delete,
      which may cause a memory leak in case of error, but will at least not cause
      a seg fault. */
      throw;
    }
    return retval;
  }

  // Protected constructor which adds the current object to the registry of IFileSvc objects.
  IFileSvc::IFileSvc() {}

}
