#ifndef st_app_StGui_h
#define st_app_StGui_h

#include <list>
#include <map>

#include "hoops/hoops.h"
#include "st_graph/Engine.h"
#include "st_graph/IEventReceiver.h"
#include "st_graph/ITabFolder.h"
#include "st_graph/Placer.h"
#include "st_stream/StreamFormatter.h"

namespace st_app {

  class AppParGroup;
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

      const std::string & getName() const;

      const std::string & getValue() const;

    protected:
      long entryWidth(hoops::IPar * par) const;

    private:
      st_graph::Engine & m_engine;
      std::string m_value_string;
      st_graph::IFrame * m_frame;
      st_graph::IFrame * m_label;
      st_graph::IFrame * m_value;
      st_graph::IFrame * m_open;
      hoops::IPar * m_par;
      bool m_stretch;
  };

  class StEventReceiver : public st_graph::IEventReceiver {
    public:
      typedef std::map<std::string, ParWidget *> ParWidgetCont;
      typedef std::map<std::string, st_graph::ITabFolder *> TabFolderCont;

      StEventReceiver(st_graph::Engine & engine, AppParGroup & par_group, StApp * m_app);

      virtual ~StEventReceiver();

      virtual void clicked(st_graph::IFrame * f);

      virtual void closeWindow(st_graph::IFrame * f);

      virtual void layout(st_graph::IFrame *);

      virtual void run();

      virtual void createMainFrame();

      virtual ParWidget * createParWidget(hoops::IPar * par, st_graph::IFrame * parent);

    private:
      bool parseRange(const hoops::IPar * par, std::list<std::string> & range);
      void getParent(const hoops::IPar * par, std::list<st_graph::IFrame *> & parent);

      st_stream::StreamFormatter m_os;
      st_graph::Engine & m_engine;
      AppParGroup & m_par_group;
      ParWidgetCont m_par_widget;
      TabFolderCont m_tab_folder;
      std::map<st_graph::IFrame *, st_graph::IFrame *> m_parent;
      st_graph::IFrame * m_main;
      st_graph::IFrame * m_group_frame;
      st_graph::IFrame * m_run;
      st_graph::IFrame * m_cancel;
      StApp * m_app;
      ParWidget * m_widest;
      long m_tab_height;
  };

  typedef StEventReceiver StAppGui;
}

#endif
