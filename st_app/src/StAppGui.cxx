/** \file StAppGui.cxx
    \brief Implementation of StAppGui class.
    \author James Peachey, HEASARC
*/
#include <algorithm>
#include <list>
#include "stdexcept"

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/StGui.h"

#include "st_graph/Engine.h"
#include "st_graph/IEventReceiver.h"
#include "st_graph/IFrame.h"
#include "st_graph/Placer.h"

#include "st_stream/StreamFormatter.h"

using namespace st_graph;

namespace st_app {

  ParWidget::ParWidget(Engine & engine, IFrame * parent, hoops::IPar * par): m_engine(engine),
    m_value_string(), m_frame(0), m_label(0), m_value(0), m_open(0), m_par(par) {
    if (0 == m_par) throw std::logic_error("ParWidget constructor was passed a null parameter pointer");

    m_value_string = m_par->Value();

    m_frame = engine.createComposite(parent, this);

    std::string label = m_par->Name();

    // Get min/max values.
    const std::string & min(m_par->Min());
    const std::string & max(m_par->Max());

    // Handle enumerated list of allowed values.
    if (max.empty() && std::string::npos != min.find("|")) {
      label += " [" + min + "]";
    } else if (!min.empty()) {
      label += " [" + min + ", " + max + "]";
    }

    m_label = engine.createLabel(m_frame, this, label);

    // If prompt was supplied, use it to create tool tip.
    const std::string & prompt(m_par->Prompt());
    if (!prompt.empty()) m_label->setToolTipText(prompt);

    // Build width of whole widget from constituent widths.
    // Width = width of label + ...
    long width = m_label->getWidth();

    if (std::string::npos != m_par->Type().find("b")) {
      // If boolean parameter, use a checkbox.
      m_value = engine.createButton(m_frame, this, "check", "");

      // Set button in state consistent with parameter value.
      bool state = *m_par;
      if (state) m_value->setState("down"); else m_value->setState("up");
    } else {
      // For all other non-boolean parameters, use a text edit.
      m_value = engine.createTextEntry(m_frame, this, m_value_string);
      if (std::string::npos != m_par->Type().find("f")) {
        // File types have additional option of a file dialog box, activated by an "Open" button.
        m_open = m_engine.createButton(m_frame, this, "text", "Open");
        width += m_open->getWidth() + 3;
      }
    }
    width += m_value->getWidth() + 3;

    // Make certain widget will not shrink smaller than the label size.
    m_frame->setMinimumWidth(m_label->getWidth());

    m_value->setWidth(parent->getWidth() / 2);
    m_frame->setWidth(width);
    m_frame->setHeight(std::max(m_label->getHeight(), m_value->getHeight()));
  }

  ParWidget::~ParWidget() { delete m_frame; }

  void ParWidget::layout(IFrame *) {
    TopEdge(m_value).below(TopEdge(m_frame));
    Center(m_label).below(Center(m_frame));
    if (0 != m_open) TopEdge(m_open).below(TopEdge(m_frame));

    LeftEdge(m_label).rightOf(LeftEdge(m_frame));
    if (0 == m_open) {
      LeftEdge(m_value).rightOf(RightEdge(m_label), 3);
    } else {
      LeftEdge(m_open).rightOf(RightEdge(m_label), 3);
      LeftEdge(m_value).rightOf(RightEdge(m_open), 3);
    }
    RightEdge(m_value).stretchTo(RightEdge(m_frame));
  }

  void ParWidget::clicked(IFrame * f) {
    if (m_open == f) {
      if (std::string::npos == m_par->Type().find("w"))
        m_value_string = m_engine.fileDialog(m_frame, ".", "open");
      else
        m_value_string = m_engine.fileDialog(m_frame, ".", "save");
      m_value->setState(m_value_string);
    } else if (m_value == f) {
      const std::string & state = m_value->getState();
      if (state == "down") m_value_string = "true";
      else if (state == "up") m_value_string = "false";
    } else {
      m_value_string = m_value->getState();
    }
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

  ParWidget::ParWidget(Engine & engine, hoops::IPar * par): m_engine(engine),
    m_value_string(), m_frame(0), m_label(0), m_value(0), m_open(0), m_par(par) {}

  hoops::IParGroup & operator <<(hoops::IParGroup & group, const ParWidget & par) {
    group[par.getName()] = par.getValue();
    return group;
  }

  StEventReceiver::StEventReceiver(StApp * app): m_os(app->getName(), "StEventReceiver", 2),
    m_engine(Engine::instance()), m_par_widget(), m_main(0), m_group_frame(0), m_run(0), m_cancel(0),
    m_app(app), m_widest(0) {}

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

      try {
        m_app->run();
      } catch (const std::exception & x) {
        m_os.err() << "Running the application failed: " << std::endl << x.what() << std::endl;
      }
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

  void StEventReceiver::run() {
    // Set up standard Gui main window.
    createMainFrame();

    AppParGroup & pars(m_app->getParGroup(m_app->getName()));
    for (hoops::GenParItor itor = pars.begin(); itor != pars.end(); ++itor) {
      // Changing from GUI to command line mode is not permitted. Also, mode is irrelevant.
      // Skip blank lines as well.
      if ((*itor)->Name() == "gui" || (*itor)->Name() == "mode") continue;
      else if (0 == (*itor)->Name().size()) continue;

      // Create widget representing each parameter.
      ParWidget * widget = createParWidget(*itor);

      // Store widget in container.
      m_par_widget.push_back(widget);

      // Keep track of the widest widget.
      if (0 == m_widest || widget->getLabel()->getWidth() > m_widest->getLabel()->getWidth()) m_widest = widget;
    }

    m_group_frame->setMinimumWidth(m_widest->getFrame()->getWidth() + 12);

    m_engine.run();
  }

  void StEventReceiver::createMainFrame() {
    // Use the name and version of the tool as a label for the GUI window.
    std::string label(m_app->getName());
    if (!label.empty()) label += " ";
    const std::string & version(m_app->getVersion());
    if (!version.empty()) label += "version " + version;

    m_main = m_engine.createMainFrame(this, 600, 400, label);
    m_group_frame = m_engine.createGroupFrame(m_main, this, "Parameters");
    m_run = m_engine.createButton(m_main, this, "text", "Run");
    m_cancel = m_engine.createButton(m_main, this, "text", "Cancel");

    // Set up some tool tips.
    m_run->setToolTipText("Run the application from inside the GUI");
    m_cancel->setToolTipText("Exit the application and GUI");

    // Disable prompting.
    AppParGroup & pars(m_app->getParGroup(m_app->getName()));
    pars.suppressPrompts();
  }

  ParWidget * StEventReceiver::createParWidget(hoops::IPar * par) {
    return new ParWidget(m_engine, m_group_frame, par);
  }

}
