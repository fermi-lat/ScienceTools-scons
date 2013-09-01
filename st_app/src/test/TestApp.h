/** \file TestApp.h
    \brief Simple test application to demonstrate how to use st_app.
    \author James Peachey, HEASARC
*/
#ifndef st_app_TestApp_h
#define st_app_TestApp_h

#include "st_app/IApp.h"

namespace test_application {

  /** \class TestApp
      \brief Test application.
  */
  class TestApp : public st_app::IApp {
    public:
      /** \brief Create TestApp object, identifying it to the base class.
      */
      TestApp(): st_app::IApp("test_application") {}

      /** \brief Perform the action needed by this application. This will be called by the standard main.
      */
      virtual void run();
  };

}

#endif
