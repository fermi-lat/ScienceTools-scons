#ifndef st_app_StGui_h
#define st_app_StGui_h

#include <list>

#include "hoops/hoops.h"
#include "st_graph/Engine.h"
#include "st_graph/IEventReceiver.h"
#include "st_graph/Placer.h"
#include "st_stream/StreamFormatter.h"

namespace st_app {

  class StApp;

  class ParWidget : public st_graph::IEventReceiver {
    public:
      ParWidget(st_graph::Engine & engine, st_graph::IFrame * parent, hoops::IPar * par);

      ~ParWidget();

      virtual void layout(st_graph::IFrame *);

      virtual void clicked(st_graph::IFrame *);

      virtual void modified(st_graph::IFrame *, const std::string & text);

      operator st_graph::IFrame *();

      st_graph::IFrame * getFrame();

      st_graph::IFrame * getLabel();

      void alignLabel(const st_graph::HPlacer & h);

      const std::string & getName() const;

      const std::string & getValue() const;

    protected:
      ParWidget(st_graph::Engine & engine, hoops::IPar * par);

    private:
      st_graph::Engine & m_engine;
      std::string m_value_string;
      st_graph::IFrame * m_frame;
      st_graph::IFrame * m_label;
      st_graph::IFrame * m_value;
      st_graph::IFrame * m_open;
      hoops::IPar * m_par;
  };

  class StEventReceiver : public st_graph::IEventReceiver {
    public:
      StEventReceiver(StApp * m_app);

      virtual ~StEventReceiver();

      virtual void clicked(st_graph::IFrame * f);

      virtual void closeWindow(st_graph::IFrame * f);

      virtual void layout(st_graph::IFrame *);

      virtual void run();

      virtual void createMainFrame();

      virtual ParWidget * createParWidget(hoops::IPar * par);

    private:
      st_stream::StreamFormatter m_os;
      st_graph::Engine & m_engine;
      std::list<ParWidget *> m_par_widget;
      st_graph::IFrame * m_main;
      st_graph::IFrame * m_group_frame;
      st_graph::IFrame * m_run;
      st_graph::IFrame * m_cancel;
      StApp * m_app;
      ParWidget * m_widest;
  };

}

#endif
