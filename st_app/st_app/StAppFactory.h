/** \file StAppFactory.h
    \brief Factory class for Science Tools application objects derived from StApp.
    \author James Peachey, HEASARC
*/
#ifndef st_app_StAppFactory_h
#define st_app_StAppFactory_h

#include <string>

#include "st_app/StAppGui.h"

namespace st_app {

  class StApp;

  /** \class IStAppFactory
      \brief Singleton base class for factory.
  */
  class IStAppFactory {
    public:
      /** \brief Return the singleton application factory.
      */
      static IStAppFactory & instance();

      virtual ~IStAppFactory() throw();

      /** \brief Create an application object.
      */
      virtual StApp * createApp() const = 0;

      /** \brief Attempt to read standard universal parameters and use them to initialize
                 error streams, debugging mode etc. If these parameters are missing, sensible
                 default values are used.
      */
      virtual void configureApp();

      virtual int getMaximumChatter() const;

      virtual void setMaximumChatter(int maximum_chatter);

      /** \brief Return true if debugging mode is enabled, false if not.
      */
      virtual bool getDebugMode() const;

      virtual void setDebugMode(bool debug_mode);

      const std::string & getAppName() const;

    protected:
      /** \brief Create factory with no application name. Universal parameters will not be read.
      */
      IStAppFactory();

      /** \brief Create factory with given application name. The application name is used
                 by configureApp to find a parameter file for universal parameters.
      */
      IStAppFactory(const std::string & app_name);

    private:
      static IStAppFactory * s_factory;

    protected:
      std::string m_app_name;
      bool m_gui_mode;
  };

  /** \class StAppFactory
      \brief Factory which creates objects of a specific subclass of StApp.
  */
  template <typename App, typename GuiApp = StAppGui>
  class StAppFactory : public IStAppFactory {
    public:
      /** \brief Create factory for an unnamed application.
      */
      StAppFactory(): IStAppFactory() {}

      /** \brief Create factory for a named application.
      */
      StAppFactory(const std::string & app_name): IStAppFactory(app_name) {}

      /** \brief Create an application object.
      */
      virtual StApp * createApp() const {
        StApp * app = 0;
        if (m_gui_mode) {
          app = new GuiApp(getAppName(), new App);
        } else {
          app = new App;
          if (0 != app) app->setName(getAppName());
        }
        return app;
      }
  };

}

#endif
