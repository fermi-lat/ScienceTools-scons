/** \file st_app_example1.cxx
    \brief "Hello world" application showing how IApp can be used as a base class for the application object.
*/

#include <iostream>

#include "st_app/IApp.h"

class AppExample1 : public st_app::IApp {
  public:
    /** \brief Create AppExample1 object, identifying it to the base class.
    */
    AppExample1(): st_app::IApp("st_app_example1") {}

    /** \brief Perform the action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      std::cout << "Hello world" << std::endl;
    }
} the_application; // Create the application object.

