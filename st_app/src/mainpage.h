/**
    \mainpage st_app package

    \author  James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    The st_app package is a utility which helps developers develop
    Science Tools applications with a common look and feel, and
    common features (e.g. -help, -version, -debug, -gui, etc.) which
    can evolve separately from the specific applications themselves.

    \section see_also See Also
    The st_app_examples package, which contains examples of how to
    use st_app, using the patterns described here.
    (Currently st_app_examples resides in users/peachey.)

    <hr>
    \section info Information For Clients
    This section describes how client code can and should use the st_app
    classes to develop Science Tools applications. In brief, st_app
    provides a base class which the client should subclass to create a
    specific application object with the desired functionality. The
    st_app library
    also provides a standard int main(int argc, char **) (hereafter
    called simply "main()") which instantiates an object of the client's
    application class to perform the real work of the application. Lastly,
    st_app provides a templated factory class which this standard main()
    function uses to instantiate an object of the client's application
    class.

    \subsection content Content of Client Code
    The test program test_main.cxx demonstrates the basic pattern
    used to create an application with st_app. An application class
    named TestApp1 is derived from the st_app::StApp class. In addition
    a singleton factory object is created using the st_app::StAppFactory template
    class. This factory object
    will be used by the standard main() function to create and then
    run the application object at runtime.

\verbatim
/** \file test_main.cxx
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

// Factory which can create an instance of the class above.
st_app::StAppFactory<TestApp1> g_factory;
\endverbatim

    As the example shows, the only requirement for subclasses of
    st_app::StApp is that they provide
    a concrete run() method (this is abstract in the base class).
    After the standard main() uses the factory object to instantiate
    the application object, it will call the application's run()
    method to perform the work of the application, in this case,
    the ever popular "Hello world" program.

    <hr>
    \section notes Release Notes
    \section requirements Requirements
    \section jobOptions jobOptions
*/
