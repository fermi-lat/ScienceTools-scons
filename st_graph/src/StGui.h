#ifndef st_graph_StGui_h
#define st_graph_StGui_h

#include <list>
#include <map>

#include "hoops/hoops.h"
#include "st_graph/Engine.h"
#include "st_graph/IEventReceiver.h"
#include "st_graph/ITabFolder.h"
#include "st_graph/Placer.h"
#include "st_stream/StreamFormatter.h"

namespace hoops {
  class IParGroup;
}

namespace st_graph {

//  class StApp;
  class StGui;

  class ParWidget : public IEventReceiver {
    public:
      ParWidget(Engine & engine, IFrame * parent, hoops::IPar * par, StGui * gui);

      ~ParWidget();

      virtual void layout(IFrame *);

      virtual void clicked(IFrame * f);

      virtual void modified(IFrame *, const std::string & text);

      operator IFrame *();

      IFrame * getFrame();

      IFrame * getLabel();

      const std::string & getName() const;

      std::string getValue() const;

      void setValue(const std::string & string_value);

      void display(bool disp_flag = true);

    protected:
      long entryWidth(hoops::IPar * par) const;

    private:
      Engine & m_engine;
      std::string m_value_string;
      StGui * m_gui;
      IFrame * m_frame;
      IFrame * m_label;
      IFrame * m_value;
      IFrame * m_open;
      hoops::IPar * m_par;
      bool m_bool;
      bool m_stretch;
      bool m_display;
  };

  class StGui : public IEventReceiver {
    public:
      typedef std::multimap<std::string, ParWidget *> ParWidgetCont;
      typedef std::multimap<std::string, ITabFolder *> TabFolderCont;

      //StEventReceiver(Engine & engine, hoops::IParGroup & par_group, StApp * m_app);
      StGui(Engine & engine, const hoops::IParGroup & par_group);

      virtual ~StGui();

      virtual void clicked(IFrame * f);

      virtual void closeWindow(IFrame * f);

      virtual void layout(IFrame *);

      virtual void run();

      virtual void createMainFrame();

      virtual ParWidget * createParWidget(hoops::IPar * par, IFrame * parent);

      virtual void synchronizeWidgets(const std::string & par_name, const std::string & value);

    private:
      bool parseRange(const hoops::IPar * par, std::list<std::string> & range);
      void getParent(const hoops::IPar * par, std::list<IFrame *> & parent);

      st_stream::StreamFormatter m_os;
      Engine & m_engine;
      ParWidgetCont m_par_widget;
      TabFolderCont m_tab_folder;
      std::map<IFrame *, IFrame *> m_parent;
      hoops::IParGroup * m_par_group;
      IFrame * m_main;
      IFrame * m_group_frame;
      IFrame * m_run;
      IFrame * m_cancel;
      IFrame * m_show_advanced;
      IFrame * m_plot_frame;
      ParWidget * m_widest;
      long m_tab_height;
  };

//  typedef StEventReceiver StGui;
}

#endif
