/** \file TestApp1.cxx
    \brief "Hello world" application showing how StApp can be used as a base class for the application object.
*/

#include <iostream>

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

/** \class TestApp1
    \brief Application singleton for test_st_app.
*/
class TestApp1 : public st_app::StApp {
  public:
    /** \brief Perform the action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      std::cout << "Hello world" << std::endl;
    }
};

/// \brief Create factory object which can create the application:
st_app::StAppFactory<TestApp1> g_app_factory("test_st_app");
