/** \file StApp.h
    \brief Base class for Science Tools applications.
    \author James Peachey, HEASARC
*/
#ifndef st_app_StApp_h
#define st_app_StApp_h

#include <string>

namespace st_app {

  class AppParGroup;

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

      /** \brief Default constructor.
      */
      StApp();

      /** \brief Virtual constructor.
      */
      virtual ~StApp() throw();

      /** \brief Perform the action needed by this application. This will be called by the standard main.
      */
      virtual void run() = 0;

      /** \brief Launch this application's GUI. The GUI will call run.
      */
      virtual void runGui();

      /** \brief Return an object which provides the most typical kinds of parameter access.
                 The name used to find the parameter file is taken from the name member.
      */
      virtual AppParGroup & getParGroup();

      /** \brief Return an object which provides the most typical kinds of parameter access.
          \param app_name The name of this application, used to find the parameter file.
      */
      virtual AppParGroup & getParGroup(const std::string & app_name);

      /** \brief Display startup banner, with name and version of the tool.
      */
      virtual void banner() const;

      const std::string & getName() const;

      const std::string & getVersion() const;

      void setName(const std::string & name);

      void setVersion(const std::string & version);

    private:
      static int s_argc;
      static char ** s_argv;
      std::string m_name;
      std::string m_version;
      AppParGroup * m_par_group;
  };

}

#endif
