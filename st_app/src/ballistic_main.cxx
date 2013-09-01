/** \file ballistic_main.cxx
    \brief Standard main for ballistic Science Tools applications. Applications should #include this
    file, but precede the inclusion by a static definition of an object of a subclass of IApp.
    \author James Peachey, HEASARC
*/

#include <exception>
#include <iostream>

#include <stdexcept>

#include "st_app/IApp.h"

int main(int argc, char ** argv) {
  int status = 0;

  try {
    // Process command line arguments. This will throw if the real application code
    // cannot/should not start.
    st_app::IApp::processCommandLine(argc, argv);

    // Get a pointer to the singleton application. Client code must have instantiated a static
    // instance of a subclass of IApp.
    st_app::IApp * app = st_app::IApp::getApp();

    // Run the application.
    app->run();

  } catch (std::exception & x) {
    std::cerr << "Error: " << x.what() << std::endl;
    status = 1;
  }

  return status;
}
