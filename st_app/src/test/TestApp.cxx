/** \file TestApp.cxx
    \brief Simple test application (Hello world) to demonstrate how to use st_app.
    \author James Peachey, HEASARC
*/

#include <iostream>

#include "TestApp.h"

namespace test_application {

  // Define what the application does. This should be considered the real "main" of the application.
  void TestApp::run() {
    std::cout << "Hello world" << std::endl;
  }

}
