/** \file st_app_main.cxx
    \brief Standard main for ballistic Science Tools applications. Applications should #include this
    file, but precede the inclusion by a static definition of an object of a subclass of IApp.
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
  st_app::StApp * app = 0;

  try {
#ifdef TRAP_FPE
    // Add floating point exception traps.
    feenableexcept(FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW);
#endif

    // Process command line arguments. This will throw if the real application code
    // cannot/should not start.
    st_app::StApp::processCommandLine(argc, argv);

    // Create an application using the singleton factory. Client code must have instantiated a single
    // instance of a subclass of IAppFactory.
    app = st_app::IStAppFactory::instance().createApp();

    // Run the application.
    app->run();

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

  delete app;

  return status;
}
