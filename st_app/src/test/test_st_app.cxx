/** \file test_application
    \brief Sample Science Tool main. This is the same for all applications.
*/
#include "TestApp.h"

// Instantiate a static instance of this application. This is needed by the standard
// Science Tools main function. In principle this may be done anywhere but it is
// most straightforward if it is done here in the main file, to ensure that the
// top-level application object is only defined in its own application.
static test_application::TestApp s_application;

#include "st_app/ballistic_main.cxx"
