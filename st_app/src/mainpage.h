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
    called simply "main()") which calls the client's application object
    to perform the real work of the application. Thus, client code need
    only define an application subclass and link against the st_app
    library.

    \subsection content Content of Client Code
    The test program test_main.cxx demonstrates the basic pattern
    used to create an application with st_app. In this case, the
    name of the application is "test_st_app". This application is
    created by deriving an application class from st_app::IApp,
    and instantiating a single instance of this application class:

\verbatim
/** \file test_main.cxx
    \brief "Hello world" application showing how IApp can be used as a base class for the application object.
*/

#include <iostream>

#include "st_app/IApp.h"

/** \class TestApp1
    \brief Application singleton for test_st_app.
*/
class TestApp1 : public st_app::IApp {
  public:
    /** \brief Create AppExample1 object, identifying it to the base class.
    */
    TestApp1(): st_app::IApp("test_st_app") {}

    /** \brief Perform the action needed by this application. This will be called by the standard main.
    */
    virtual void run() {
      std::cout << "Hello world" << std::endl;
    }
} the_application; // Create the application object.

\endverbatim

    As the example shows, subclasses of st_app::IApp should include an explicit default
    constructor which passes the name of the application to the constructor
    for the base class st_app::IApp. The only other method which need be provided
    by the application class is the run() method, which in this example contains
    the ever popular "hello world" application behavior.

    A note of caution: although developers are being asked to derive
    application classes from st_app::IApp, it is recommended that they do not
    add any member data. Instead, it is best if MyApp::run() is treated as if it
    were the implementation main(), using only self-contained data and globally
    accessible objects defined elsewhere.

    Restated, MyApp should be viewed as a simple encapsulation of the concept
    of the application qua application, and nothing more. The client should develop
    separate classes to add required specific functionality, and MyApp::run()
    can use these classes. The MyApp class should not itself be used as a
    catch-all class in which all the application functionality is added.

    <hr>
    \section notes Release Notes
    \section requirements Requirements
    \section jobOptions jobOptions
    \section footnote Footnote
*/
