/** \file StAppGui.h
    \brief Declaration of StAppGui class.
    \author James Peachey, HEASARC
*/
#ifndef st_app_StAppGui_h
#define st_app_StAppGui_h

#include <string>

#include "st_app/StApp.h"

namespace st_app {

  /** \class StAppGui
      \brief Standard parameter file-driven GUI for Science Tools.
  */
  class StAppGui : public StApp {
    public:
      /** \brief Construct a gui app which refers to the given application of the given name.
          \param app_name The name of the application object being run by this GUI.
          \param app The actual application object being run by this GUI.
      */
      StAppGui(const std::string & app_name, StApp * app);

      virtual ~StAppGui() throw();

      /** \brief Perform the action needed by this application. This will be called by the standard main.
      */
      virtual void run();

    private:
      StApp * m_app;
  };

}

#endif
