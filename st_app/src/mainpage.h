/**
    \mainpage st_app package

    \author  James Peachey peachey@lheamail.gsfc.nasa.gov

    \section intro Introduction
    The st_app package is a utility which helps developers develop
    Science Tools applications with a common look and feel, and
    common features (e.g. -help, -version, -debug, etc.) which
    can evolve separately from the specific applications themselves.

    \section see_also See Also
    The st_app_examples package, which contains examples of how to
    use st_app, using the patterns described here.

    <hr>
    \section info Information For Clients
    This section describes how client code can and should use the st_app
    classes to develop Science Tools applications. In brief, st_app
    provides a base class which the client should subclass to create a
    specific application object with the
    desired functionality. St_app also provides a standard
    int main(int argc, char **) (hereafter called simply "main()")
    which calls the client's application
    object to perform the real work of the application. The client
    therefore must also provide a separate implementation file which
    includes this standard main() function.

    \subsection content Content of Client Code
    The following explanation assumes that the client is creating
    an application named "my_app". The client will do this by
    providing a class called "MyApp" which does the actual work of
    the application. This requires that three files be created.
    The content of these files will be discussed in this section.
    The recommended placement of these files within the client code
    will be discussed in the next section.

    The first step is for the client to create a file my_app.cxx
    for their application's main() using the following pattern:

    \verbatim
#include "my_app/MyApp.h"

static my_app::MyApp s_application;

#include "st_app/ballistic_main.cxx"
    \endverbatim

    The first line includes the client's header file (see next step)
    which declares the client class MyApp. The second instantiates
    a static application object of type MyApp. This is the object
    that st_app's standard main() function will use. The third
    line includes st_app's standard main() function, which performs
    standard initial steps before using the application object to
    do the specific work of the application.

    The next step is for the client to create a header file MyApp.h
    to hold the declaration of his/her application class, using the
    following pattern:

    \verbatim
#ifndef my_app_MyApp_h
#define my_app_MyApp_h

#include "st_app/IApp.h"

namespace my_app {

  class MyApp : public st_app::IApp {
    public:
      MyApp(): st_app::IApp("my_app") {}

      virtual void run();
  };

}

#endif
    \endverbatim

    As the example shows, the client must provide an explicit default
    constructor for MyApp, which call's st_app::IApp's constructor,
    passing it as an argument a string with the name of the application.
    The client class also must provide the virtual void run() method.

    The last step is for the client to provide an implementation file
    MyApp.cxx using the following pattern:

    \verbatim
#include "my_app/MyApp.h"

namespace my_app {

  // This method acts as the real "main" of the application.
  void MyApp::run() {
    // Client adds code here to do the actual work of the application.
  }

}
    \endverbatim

    A note of caution: although developers are being asked to derive
    application classes from st_app::IApp, it is recommended that they do not
    add any new member functions or member data. Instead, it is best if
    MyApp::run() is treated as if it were the implementation main(),
    using only self-contained data and globally accessible objects defined
    elsewhere. The reason for this is that eventually it may be possible for complex
    applications to include more than one object derived from st_app::IApp.
    To minimize the risk of side-effects, it is best if each run() (think main()) is
    completely self-contained, and does not instantiate any objects until it
    is called. Also, it is good OO practice in general to keep classes small
    and specific.

    Restated, MyApp should be viewed as a simple encapsulation of the concept
    of the application itself, and nothing more. The client should develop
    separate classes to add required specific functionality, and MyApp::run()
    can use these classes. The MyApp class should not itself be used as a
    catch-all class in which the application functionality is added. See footnote.

    \subsection layout Layout of Client Code
    In this example, the code described above would best be
    laid out in the following way (under the my_app/vVrRpP directory):

    \verbatim
  .
  |-- my_app
  |   `-- MyApp.h
  `-- src
      |-- MyApp.cxx
      `-- my_app
          `-- my_app.cxx
    \endverbatim

    The client is of course free to change the names of these files,
    although in the absence of a reason to change them, it would make it
    easiest for anyone browsing the Science Tools if this scheme would be
    used consistently. It is strongly recommended, however, that this placement
    be used, for the following reasons.

    The application code is completely separate from the main() function, and
    follows the standard Science Tools directory structure. This allows
    it to reside in a library naturally using standard cmt patterns. This
    is useful even if currently the application does not use a library; it
    promotes flexibilty should the developer ever wish to create a library.

    By contrast, the main() function is in an application-specific directory, which allows
    it to be used only for creating an executable containing the application
    code naturally using standard cmt patterns.

    \subsection mulitple_apps What if the client has more than one application?
    In the event of multiple applications within a single package,
    the pattern naturally generalizes to:

    \verbatim
  .
  |-- my_applications
  |   `-- MyApp1.h
  |   `-- MyApp2.h
  `-- src
      |-- MyApp1.cxx
      |-- MyApp2.cxx
      |-- my_app1
      |   `-- my_app1.cxx
      `-- my_app2
          `-- my_app2.cxx
    \endverbatim

    This is the pattern in use in the st_app_examples package.

    \subsection general_notes Some General Notes
    Some more comments.

    The underlying purpose of this structure is to provide maximum flexibility
    and code reuse, by keeping as much of the application code as possible
    in a library-friendly state.

    The use of st_app's standard main() function allows st_app to add generic
    features to every application without the application code needing to
    change. For example, the st_app can intercept an argument of -help and
    cause this to print the help for the application without even running
    the application.

    <hr>
    \section notes Release Notes
    \section requirements Requirements
    \section jobOptions jobOptions
    \section footnote Footnote
    Example here of good and bad code, showing one clean MyApp and one where
    the developer just throws in the kitchen sink.
*/
