/** \file ballistic_main.cxx
    \brief Standard main for ballistic Science Tools applications. Applications should #include this
    file, but precede the inclusion by a static definition of an object of a subclass of IApp.
    \author James Peachey, HEASARC
*/

#include <iostream>
#include <stdexcept>
#include <typeinfo>

#include "st_app/IApp.h"

int main(int argc, char ** argv) {
  int status = 0;

  try {
    // Process command line arguments. This will throw if the real application code
    // cannot/should not start.
    st_app::IApp::processCommandLine(argc, argv);

    // Get a pointer to the singleton application. Client code must have instantiated a single
    // instance of a subclass of IApp.
    st_app::IApp * app = st_app::IApp::getApp();

    // Run application's setup code, if any.
    app->setUp();

    // Run the application.
    app->run();

    // Run application clean-up code, if any.
    app->tearDown();

  } catch (const std::exception & x) {
    // Return a non-zero exit code:
    status = 1;

    // Report the type of the exception if possible, using typeid; typeid can throw so be careful:
    try {
      const char * info = typeid(x).name();
      std::cerr << "Caught " << info << " at the top level: " << x.what() << std::endl;
    } catch (...) {
      std::cerr << "Caught std::exception at the top level: " << x.what() << std::endl;
    }
  }

  return status;
}
