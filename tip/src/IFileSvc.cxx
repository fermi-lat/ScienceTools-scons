/** \file IFileSvc.cxx

    \brief Factory for handling table objects.

    \author James Peachey, HEASARC
*/

#include <fstream>

#include "FitsExtensionData.h"
#include "FitsFileManager.h"
#include "RootExtensionData.h"
#include "RootExtensionManager.h"
#include "tip/Extension.h"
#include "tip/FileSummary.h"
#include "tip/IFileSvc.h"
#include "tip/Image.h"
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
    FitsFileManager::createFile(file_name, template_name);
  }

  // TODO 11: read/edit Extension/Table/Image is getting cumbersome; lots of similar methods,
  // duplicated code. In addition, error messages for files which cannot be opened tend to be
  // confusing. For instance, if bozo.fits is not present, one sees two errors: Fits file bozo.fits
  // can't be opened, and Root file bozo.fits can't be opened.
  // So: 1) Add a method which determines whether a file exists, and what type it is, and change
  // editTable so it only tries to open the correct file type. 2) Find some reasonable way to merge
  // the various edit/read methods so that there is no duplicate code, and preferably so that there
  // are fewer methods overall.

  // Open read-write an extension in a file, be it FITS or Root, table or image.
  Extension * IFileSvc::editExtension(const std::string & file_name, const std::string & ext_name,
    const std::string & filter) {
    Extension * retval = 0;
    IExtensionData * data = 0;
    TipException fits_exception;
    try {
      try {
        // Open file with read-only mode enabled.
        data = new FitsExtensionData(file_name, ext_name, filter, false);
      } catch(const TipException & x) {
        fits_exception = x;
        data = new RootExtensionData(file_name, ext_name, filter);
      }

      // Determine whether this extension is a table or an image, and return the appropriate
      // type of object..
      if (data->isTable())
        retval = new Table(data);
      else
        retval = new Image(data);

    } catch(const TipException & x) {
      delete retval; // If retval is non-0, Extension was created, so it will delete data.
      throw TipException(std::string(fits_exception.what()) + "\n" + x.what());
    } catch(...) {
      delete retval; // If retval is non-0, Extension was created, so it will delete data.
      throw;
    }
    return retval;
  }

  // Edit a table in a file, be it FITS or Root.
  Table * IFileSvc::editTable(const std::string & file_name, const std::string & table_name,
    const std::string & filter) {
    Table * retval = 0;
    IExtensionData * data = 0;
    TipException fits_exception;
    try {
      try {
        data = new FitsExtensionData(file_name, table_name, filter, false);
      } catch(const TipException & x) {
        fits_exception = x;
        data = new RootExtensionData(file_name, table_name, filter);
      }
      retval = new Table(data);
    } catch(const TipException & x) {
      delete retval; // If retval is non-0, Table was created, so it will delete data.
      throw TipException(std::string(fits_exception.what()) + "\n" + x.what());
    } catch(...) {
      delete retval; // If retval is non-0, Table was created, so it will delete data.

      /*TODO 1: 4/2/2004: Memory management problem: Extension is base of Table. Extension
      has a IExtensionData and ~Extension deletes it. Currently editTable creates
      the IExtensionData and passes it to Table::Table(...) which passes it to
      Extension::Extension(...). If something throws along the way, catch 22: If
      Extension throws, editTable should delete the IExtensionData because
      ~Extension wont be called. If Table throws, editTable shouldn't delete it
      because ~Extension *will* be called. FOR NOW: take out editTable's delete,
      which may cause a memory leak in case of error, but will at least not cause
      a seg fault. */

      /* DONE 1: 4/21/2004: This is not an issue at present, because Table::Table doesn't
      throw under any circumstance. */
      throw;
    }
    return retval;
  }

  // Read-only an extension in a file, be it FITS or Root, table or image.
  const Extension * IFileSvc::readExtension(const std::string & file_name, const std::string & table_name,
    const std::string & filter) {
    Extension * retval = 0;
    IExtensionData * data = 0;
    TipException fits_exception;
    try {
      try {
        // Open file with read-only mode enabled.
        data = new FitsExtensionData(file_name, table_name, filter, true);
      } catch(const TipException & x) {
        fits_exception = x;
        data = new RootExtensionData(file_name, table_name, filter);
      }

      // Determine whether this extension is a table or an image, and return the appropriate
      // type of object..
      if (data->isTable())
        retval = new Table(data);
      else
        retval = new Image(data);

    } catch(const TipException & x) {
      delete retval; // If retval is non-0, Extension was created, so it will delete data.
      throw TipException(std::string(fits_exception.what()) + "\n" + x.what());
    } catch(...) {
      delete retval; // If retval is non-0, Extension was created, so it will delete data.
      throw;
    }
    return retval;
  }

  // Read-only a table in a file, be it FITS or Root.
  const Table * IFileSvc::readTable(const std::string & file_name, const std::string & table_name,
    const std::string & filter) {
    Table * retval = 0;
    IExtensionData * data = 0;
    TipException fits_exception;
    try {
      try {
        // Open file with read-only mode enabled.
        data = new FitsExtensionData(file_name, table_name, filter, true);
      } catch(const TipException & x) {
        fits_exception = x;
        data = new RootExtensionData(file_name, table_name, filter);
      }
      retval = new Table(data);
    } catch(const TipException & x) {
      delete retval; // If retval is non-0, Table was created, so it will delete data.
      throw TipException(std::string(fits_exception.what()) + "\n" + x.what());
    } catch(...) {
      delete retval; // If retval is non-0, Table was created, so it will delete data.
      throw;
    }
    return retval;
  }

  void IFileSvc::getFileSummary(const std::string & file_name, FileSummary & summary) {
    FitsFileManager::getFileSummary(file_name, summary);
  }

  bool IFileSvc::fileExists(const std::string & file_name) {
    std::ifstream file(file_name.c_str());
    if (!file.is_open()) return false;
    return true;
  }

  // Protected constructor which adds the current object to the registry of IFileSvc objects.
  IFileSvc::IFileSvc() {}

}
