/** \file IApp.h
    \brief Base class for Science Tools applications.
    \author James Peachey, HEASARC
*/
#ifndef st_app_IApp_h
#define st_app_IApp_h

#include <string>

namespace hoops {
  class IParGroup;
  class ParPromptGroup;
}

namespace st_app {

  /** \class IApp
      \brief Application of standard type.
  */
  class IApp {
    public:
      /** \brief Return the main singleton application.
      */
      static IApp * getApp();

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

      virtual ~IApp() throw();

      /** \brief Get name of this component.
      */
      const std::string & name() const;

      /** \brief Perform the action needed by this application. This will be called by the standard main.
      */
      virtual void run() = 0;

      /** \brief Get the group of parameters from hoops.
      */
      hoops::IParGroup & hoopsGetParGroup();

      /** \brief Use hoops to prompt for parameters for this application.
      */
      void hoopsPrompt();

      /** \brief Save the current parameters.
      */
      void hoopsSave();

    protected:
      /** \brief Create application object. The first application created has special significance
          as the top-level singleton application.
          \param app_name The name of this application.
      */
      IApp(const std::string & app_name);

    private:
      static IApp * s_application;
      static int s_argc;
      static char ** s_argv;
      std::string m_app_name;
      hoops::ParPromptGroup * m_hoops_par_group;
  };

}

#endif
