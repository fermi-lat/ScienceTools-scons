/** \file st_app_main.cxx
    \brief Standard main for ballistic Science Tools applications.
    \author James Peachey, HEASARC
*/

// Add floating point exception traps if requested.
#ifdef TRAP_FPE
#include <fenv.h>
#endif

#include <iostream>
#include <stdexcept>
#include <typeinfo>

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

int main(int argc, char ** argv) {
  int status = 0;
  st_app::StApp * this_st_app = 0;

  try {
#ifdef TRAP_FPE
    // Add floating point exception traps.
    feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
#endif

    // Process command line arguments. This will throw if the real application code
    // cannot/should not start.
    st_app::StApp::processCommandLine(argc, argv);

#ifndef ST_APP_DEBUG
    // Not in debug mode, so create and run the application here, inside the top-level try block.
    try {
      // Try using the singleton StAppFactory to create the application:
      this_st_app = st_app::IStAppFactory::instance().createApp();
    } catch(const std::logic_error & x) {
      throw std::logic_error("Failed to get an StAppFactory singleton: client must define one");
    }

    // Run the application.
    if (0 != this_st_app) this_st_app->run();
#endif

  } catch (const std::exception & x) {
    // Return a non-zero exit code:
    status = 1;

    // Report the type of the exception if possible, using typeid; typeid can throw so be careful:
    const char * type_name = "std::exception";
    try {
      type_name = typeid(x).name();
    } catch (...) {
      // Ignore problem with typeids.
    }
    std::cerr << "Caught " << type_name << " at the top level: " << x.what() << std::endl;
  }

#ifdef ST_APP_DEBUG
  // In debug mode, so create and run the application here, outside of any try-catch,
  // so that the debugger is more useful for finding exceptions at their source.
  // Use the singleton StAppFactory to create the application:
  this_st_app = st_app::IStAppFactory::instance().createApp();

  // Run the application.
  if (0 != this_st_app) this_st_app->run();
#endif

  // Clean up:
  delete this_st_app;

  return status;
}
