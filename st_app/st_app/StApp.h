/** \file StApp.h
    \brief Base class for Science Tools applications.
    \author James Peachey, HEASARC
*/
#ifndef st_app_StApp_h
#define st_app_StApp_h

namespace st_app {

  /** \class StApp
      \brief Application of standard type.
  */
  class StApp {
    public:
      /** \brief Handle command line arguments, storing the ones the application may need.
          \param argc The number of arguments in the argument array.
          \param argv The arguments array.
      */
      static void processCommandLine(int argc, char ** argv);

      /** \brief Get number of command line arguments.
      */
      static int getArgc();

      /** \brief Get command line arguments.
      */
      static char ** getArgv();

      virtual ~StApp() throw() {}

      /** \brief Perform the action needed by this application. This will be called by the standard main.
      */
      virtual void run() = 0;

    private:
      static int s_argc;
      static char ** s_argv;
  };

}

#endif
