/** \file StAppGui.cxx
    \brief Implementation of StAppGui class.
    \author James Peachey, HEASARC
*/
#include <algorithm>
#include <list>
#include "stdexcept"

#include "st_app/AppParGroup.h"
#include "st_app/StAppFactory.h"
#include "st_app/StAppGui.h"

#include "st_graph/Engine.h"
#include "st_graph/IEventReceiver.h"
#include "st_graph/IFrame.h"
#include "st_graph/Placer.h"

#include "st_stream/StreamFormatter.h"

using namespace st_graph;

namespace st_app {

  class ParWidget : public IEventReceiver {
    public:
      ParWidget(Engine & engine, IFrame * parent, hoops::IPar * par);

      ~ParWidget();

      virtual void layout(IFrame *);

      virtual void modified(IFrame *, const std::string & text);

      operator IFrame *();

      IFrame * getFrame();

      IFrame * getLabel();

      void alignLabel(const HPlacer & h);

      const std::string & getName() const;

      const std::string & getValue() const;

    private:
      std::string m_value_string;
      IFrame * m_frame;
      IFrame * m_label;
      IFrame * m_value;
      hoops::IPar * m_par;
  };

  ParWidget::ParWidget(Engine & engine, IFrame * parent, hoops::IPar * par): m_value_string(),
    m_frame(0), m_label(0), m_value(0), m_par(par) {
    if (0 == m_par) throw std::logic_error("ParWidget constructor was passed a null parameter pointer");

    m_value_string = m_par->Value();

    m_frame = engine.createComposite(parent, this);
    m_label = engine.createLabel(m_frame, this, m_par->Name());
    m_value = engine.createTextEntry(m_frame, this, m_value_string);

    // Make certain widget will not shrink smaller than the label size.
    m_frame->setMinimumWidth(m_label->getWidth());

    m_value->setWidth(parent->getWidth() / 2);
    m_frame->setWidth(m_label->getWidth() +  m_value->getWidth() + 3);
    m_frame->setHeight(std::max(m_label->getHeight(), m_value->getHeight()));
  }

  ParWidget::~ParWidget() { delete m_frame; }

  void ParWidget::layout(IFrame *) {
    TopEdge(m_value).below(TopEdge(m_frame));
    Center(m_label).below(Center(m_frame));

    LeftEdge(m_label).rightOf(LeftEdge(m_frame));
    LeftEdge(m_value).rightOf(RightEdge(m_label), 3);
    RightEdge(m_value).stretchTo(RightEdge(m_frame));
  }

  void ParWidget::modified(IFrame *, const std::string & text) {
    m_value_string = text;
  }

  ParWidget::operator IFrame * () { return getFrame(); }

  IFrame * ParWidget::getFrame () { return m_frame; }

  IFrame * ParWidget::getLabel() { return m_label; }

  const std::string & ParWidget::getName() const { return m_par->Name(); }

  const std::string & ParWidget::getValue() const { return m_value_string; }

  void ParWidget::alignLabel(const HPlacer & h) {
    RightEdge(m_label).leftOf(h);
    LeftEdge(m_value).rightOf(RightEdge(m_label), 3);
    LeftEdge(m_frame).leftOf(LeftEdge(m_label));
    RightEdge(m_frame).stretchTo(RightEdge(m_value));
  }

  hoops::IParGroup & operator <<(hoops::IParGroup & group, const ParWidget & par) {
    group[par.getName()] = par.getValue();
    return group;
  }

  class StEventReceiver : public IEventReceiver {
    public:
      StEventReceiver(StApp * m_app);

      virtual ~StEventReceiver();

      virtual void clicked(IFrame * f);

      virtual void closeWindow(IFrame * f);

      virtual void layout(IFrame *);

    private:
      st_stream::StreamFormatter m_os;
      Engine & m_engine;
      std::list<ParWidget *> m_par_widget;
      IFrame * m_main;
      IFrame * m_group_frame;
      IFrame * m_run;
      IFrame * m_cancel;
      StApp * m_app;
      ParWidget * m_widest;
  };

  StEventReceiver::StEventReceiver(StApp * app): m_os(app->getName(), "StEventReceiver", 2),
    m_engine(Engine::instance()), m_par_widget(), m_main(0), m_group_frame(0), m_run(0), m_cancel(0),
    m_app(app), m_widest(0) {
    m_main = m_engine.createMainFrame(this, 600, 400, app->getName() + " Version " + app->getVersion());
    m_group_frame = m_engine.createGroupFrame(m_main, this, "Parameters");
    m_run = m_engine.createButton(m_main, this, "text", "Run");
    m_cancel = m_engine.createButton(m_main, this, "text", "Cancel");

    AppParGroup & pars(m_app->getParGroup(m_app->getName()));

    pars.suppressPrompts();

    for (hoops::GenParItor itor = pars.begin(); itor != pars.end(); ++itor) {
      // Changing from GUI to command line mode is not permitted.
      if ((*itor)->Name() == "gui") continue;

      ParWidget * widget = new ParWidget(m_engine, m_group_frame, *itor);
      m_par_widget.push_back(widget);
      if (0 == m_widest || widget->getLabel()->getWidth() > m_widest->getLabel()->getWidth()) m_widest = widget;
    }

    m_group_frame->setMinimumWidth(m_widest->getFrame()->getWidth() + 12);

    m_engine.run();
  }

  StEventReceiver::~StEventReceiver() {
    for (std::list<ParWidget *>::reverse_iterator itor = m_par_widget.rbegin(); itor != m_par_widget.rend(); ++itor) delete (*itor);
    delete m_main;
  }

  void StEventReceiver::clicked(IFrame * f) {
    if (f == m_run) {
      AppParGroup & pars(m_app->getParGroup(m_app->getName()));

      try {
        for (std::list<ParWidget *>::iterator itor = m_par_widget.begin(); itor != m_par_widget.end(); ++itor) {
          pars << *(*itor);
        }

        pars.Save();
      } catch (const std::exception & x) {
        m_os.err() << "Problem with parameter: " << x.what() << std::endl;
        return;
      }

      try {
        int chatter = pars["chatter"];
        IStAppFactory::instance().setMaximumChatter(chatter);
      } catch (const std::exception &) {
        // Ignore
      }

      try {
        bool debug = pars["debug"];
        IStAppFactory::instance().setDebugMode(debug);
      } catch (const std::exception &) {
        // Ignore
      }

      m_app->run();
    } else if (f == m_cancel) {
      m_engine.stop();
    }
  }

  void StEventReceiver::closeWindow(IFrame * f) {
    if (f == m_main) m_engine.stop();
  }

  void StEventReceiver::layout(IFrame * f) {
    // Only layout the main frame.
    if (f != m_main) return;

    // Stack buttons horizontally at the top of the frame.
    LeftEdge(m_run).rightOf(LeftEdge(m_main), 6);
    LeftEdge(m_cancel).rightOf(RightEdge(m_run), 6);

    TopEdge(m_run).below(TopEdge(m_main));
    TopEdge(m_cancel).below(TopEdge(m_main));

    // Size the group frame so that it sits nicely below the buttons.
    TopEdge(m_group_frame).below(BottomEdge(m_cancel), 6);
    BottomEdge(m_group_frame).stretchTo(BottomEdge(m_main), -6);
    LeftEdge(m_group_frame).rightOf(LeftEdge(m_main), 6);
    RightEdge(m_group_frame).stretchTo(RightEdge(m_main), -6);

    // Place the widget with the widest label at the left edge of the frame.
    LeftEdge(*m_widest).rightOf(LeftEdge(m_group_frame), 3);

    std::list<ParWidget *>::iterator itor = m_par_widget.begin();
    std::list<ParWidget *>::iterator next = itor;

    if (itor != m_par_widget.end()) {

      TopEdge(*(*itor)).below(TopEdge(m_group_frame), 22);
      (*itor)->alignLabel(RightEdge(m_widest->getLabel()));
      RightEdge(*(*itor)).stretchTo(RightEdge(m_group_frame), -10);

      // Stack and align widgets.
      for (++next; next != m_par_widget.end(); ++itor, ++next) {
        TopEdge(*(*next)).below(BottomEdge(*(*itor)));
        (*next)->alignLabel(RightEdge(m_widest->getLabel()));
        RightEdge(*(*next)).stretchTo(RightEdge(m_group_frame), -10);
      }

      // itor points to the lowest parameter widget, so extend group frame to that point.
      BottomEdge(m_group_frame).stretchTo(BottomEdge(*(*itor)), +10);
    }
  }

  StAppGui::StAppGui(const std::string & app_name, StApp * app): m_app(app) {
    if (0 == m_app) throw std::logic_error("StAppGui constructor was passed a null application pointer");
    m_app->setName(app_name);
  }

  StAppGui::~StAppGui() throw() { delete m_app; }

  void StAppGui::run() {
    StEventReceiver event_receiver(m_app);
  }

}
