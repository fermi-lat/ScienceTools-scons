/** \file StAppFactory.h
    \brief Factory class for Science Tools application objects derived from StApp.
    \author James Peachey, HEASARC
*/
#ifndef st_app_StAppFactory_h
#define st_app_StAppFactory_h

namespace st_app {

  class StApp;

  /** \class IStAppFactory
      \brief Singleton base class for factory.
  */
  class IStAppFactory {
    public:
      static IStAppFactory & instance();

      virtual ~IStAppFactory() throw();
      virtual StApp * createApp() = 0;

    protected:
      IStAppFactory();

    private:
      static IStAppFactory * s_factory;
  };

  template <typename App>
  class StAppFactory : public IStAppFactory {
    public:
      virtual StApp * createApp() { return new App; }
  };

}

#endif
