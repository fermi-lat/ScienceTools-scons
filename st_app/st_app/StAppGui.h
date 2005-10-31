#ifndef st_app_StAppGui_h
#define st_app_StAppGui_h

#include "st_graph/StGui.h"

namespace st_graph {
  class Engine;
  class IFrame;
}

namespace st_app {

  class StApp;

  class StAppGui : public st_graph::StGui {
    public:
      StAppGui(st_graph::Engine & engine, StApp & app);

      virtual void runApp();

    protected:
      StApp * m_app;
  };

}

#endif
