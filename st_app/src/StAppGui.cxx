/** \file StAppGui.cxx
    \brief Implementation of StAppGui class.
    \author James Peachey, HEASARC
*/
#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/StGui.h"

#include "st_graph/Engine.h"
#include "st_graph/IEventReceiver.h"
#include "st_graph/IFrame.h"
#include "st_graph/ITabFolder.h"
#include "st_graph/Placer.h"

#include "st_stream/StreamFormatter.h"

using namespace st_graph;

namespace st_app {

  ParWidget::ParWidget(st_graph::Engine & engine, st_graph::IFrame * parent, hoops::IPar * par): m_engine(engine),
    m_value_string(), m_frame(0), m_label(0), m_value(0), m_open(0), m_par(par), m_stretch(false), m_display(false) {
    if (0 == m_par) throw std::logic_error("ParWidget constructor was passed a null parameter pointer");

    // Get value from parameter.
    m_value_string = m_par->Value();

    m_frame = m_engine.createComposite(parent, this);

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

    m_label = m_engine.createLabel(m_frame, this, label);

    // If prompt was supplied, use it to create tool tip.
    const std::string & prompt(m_par->Prompt());
    if (!prompt.empty()) m_label->setToolTipText(prompt);

    // Build width of whole widget from constituent widths.
    // Width = width of label + ...
    long width = m_label->getWidth();

    if (std::string::npos != m_par->Type().find("b")) {
      // If boolean parameter, use a checkbox.
      m_value = m_engine.createButton(m_frame, this, "check", "");

      // Set button in state consistent with parameter value.
      bool state = *m_par;
      if (state) m_value->setState("down"); else m_value->setState("up");
    } else {
      // For all other non-boolean parameters, use a text edit.
      m_value = m_engine.createTextEntry(m_frame, this, m_value_string);

      // Set width to the standard width for this parameter type.
      m_value->setWidth(entryWidth(m_par));

      // Special cases.
      if (std::string::npos != m_par->Type().find("f")) {
        // File types have additional option of a file dialog box, activated by an "Open" button.
        m_open = m_engine.createButton(m_frame, this, "text", "...");
        // Adjust width to include button.
        width += m_open->getWidth() + 3;
        m_stretch = true;
      } else if (std::string::npos != m_par->Type().find("s")) {
        m_stretch = true;
      }
    }
    // Adjust width to include m_value widget.
    width += m_value->getWidth() + 3;

    // Make certain widget will not shrink smaller than the label size.
    m_frame->setMinimumWidth(m_label->getWidth());

    m_frame->setWidth(width);
    m_frame->setHeight(std::max(m_label->getHeight(), m_value->getHeight()));

    // By default, "hidden" parameters are not displayed to start with.
    m_display = (std::string::npos == m_par->Mode().find("h"));
  }

  ParWidget::~ParWidget() { delete m_frame; }

  void ParWidget::layout(st_graph::IFrame * f) {
    if (f != m_frame) return;

    Center(m_value).below(Center(m_frame));
    Center(m_label).below(Center(m_frame));
    if (0 != m_open) Center(m_open).below(Center(m_frame));

    LeftEdge(m_label).rightOf(LeftEdge(m_frame));
    LeftEdge(m_value).rightOf(RightEdge(m_label), 3);

    if (0 == m_open) {
      if (m_stretch) RightEdge(m_value).stretchTo(RightEdge(m_frame));
    } else {
      RightEdge(m_open).leftOf(RightEdge(m_frame));
      if (m_stretch) RightEdge(m_value).stretchTo(RightEdge(m_open), -3);
    }

    if (!m_display) {
      m_frame->unDisplay();
      m_label->unDisplay();
      m_value->unDisplay();
      if (0 != m_open) m_open->unDisplay();
    }
  }

  void ParWidget::clicked(st_graph::IFrame * f) {
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

  void ParWidget::modified(st_graph::IFrame *, const std::string & text) {
    m_value_string = text;
  }

  ParWidget::operator st_graph::IFrame * () { return getFrame(); }

  st_graph::IFrame * ParWidget::getFrame () { return m_frame; }

  st_graph::IFrame * ParWidget::getLabel() { return m_label; }

  const std::string & ParWidget::getName() const { return m_par->Name(); }

  const std::string & ParWidget::getValue() const { return m_value_string; }

  void ParWidget::display(bool disp_flag) {
    m_display = disp_flag;
    if (m_display) {
      if (0 != m_open) m_open->display();
      m_value->display();
      m_label->display();
      m_frame->display();
    } else {
      layout(m_frame);
    }
  }

  long ParWidget::entryWidth(hoops::IPar * par) const {
    // The first time this is called, create a temporary gui with text entries corresponding to the sizes of parameters.
    static std::map<std::string, long> s_width;
    if (s_width.empty()) {
      std::auto_ptr<IFrame> mf(m_engine.createMainFrame(0, 100, 100, "sizer"));
      std::auto_ptr<IFrame> bool_pw(m_engine.createTextEntry(mf.get(), 0, "false"));
      std::auto_ptr<IFrame> int_pw(m_engine.createTextEntry(mf.get(), 0, "+1234567890"));
      std::auto_ptr<IFrame> float_pw(m_engine.createTextEntry(mf.get(), 0, "1.2345678901234E+123"));
      std::auto_ptr<IFrame> string_pw(m_engine.createTextEntry(mf.get(), 0, "1234567890123456789012345678901234567890"));
      // Store sizes of text entry boxes for each parameter type.
      s_width.insert(std::make_pair(std::string("b"), bool_pw->getWidth()));
      s_width.insert(std::make_pair(std::string("i"), int_pw->getWidth()));
      s_width.insert(std::make_pair(std::string("r"), float_pw->getWidth()));
      s_width.insert(std::make_pair(std::string("f"), string_pw->getWidth()));
      s_width.insert(std::make_pair(std::string("s"), string_pw->getWidth()));
    }

    long width = 10;
    for (std::map<std::string, long>::iterator itor = s_width.begin(); itor != s_width.end(); ++itor) {
      if (std::string::npos != par->Type().find(itor->first)) {
        width = itor->second;
        break;
      }
    }
    return width;
  }

  hoops::IParGroup & operator <<(hoops::IParGroup & group, const ParWidget & par) {
    group[par.getName()] = par.getValue();
    return group;
  }

  StEventReceiver::StEventReceiver(st_graph::Engine & engine, AppParGroup & par_group, StApp * app):
    m_os(app->getName(), "StEventReceiver", 2), m_engine(engine), m_par_group(par_group), m_par_widget(), m_tab_folder(),
    m_parent(), m_main(0), m_group_frame(0), m_run(0), m_cancel(0), m_show_advanced(0), m_app(app), m_widest(0), m_tab_height(0) {}

  StEventReceiver::~StEventReceiver() {
    for (ParWidgetCont::reverse_iterator itor = m_par_widget.rbegin(); itor != m_par_widget.rend(); ++itor)
      delete *itor->second;
    delete m_main;
  }

  void StEventReceiver::clicked(st_graph::IFrame * f) {
    if (f == m_run) {
      AppParGroup & pars(m_par_group);

      try {
        for (ParWidgetCont::iterator itor = m_par_widget.begin(); itor != m_par_widget.end(); ++itor) {
          pars << *itor->second;
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
    } else if (f == m_show_advanced) {
      // Show/hide advanced parameters.
      for (ParWidgetCont::iterator itor = m_par_widget.begin(); itor != m_par_widget.end(); ++itor) {
        // If parameter is not "hidden" do not tamper with its visibility.
        if (std::string::npos == m_par_group[itor->first].Mode().find("h")) continue;
        const std::string & state(m_show_advanced->getState());
        if (state == "up") itor->second->display(false);
        else if (state == "down") itor->second->display(true);
      }
      layout(m_group_frame);
    }
  }

  void StEventReceiver::closeWindow(st_graph::IFrame * f) {
    if (f == m_main) m_engine.stop();
  }

  void StEventReceiver::layout(st_graph::IFrame * f) {
    if (f == m_main) {
      // Stack buttons horizontally at the top of the frame.
      LeftEdge(m_run).rightOf(LeftEdge(m_main), 6);
      LeftEdge(m_cancel).rightOf(RightEdge(m_run));
      LeftEdge(m_show_advanced).rightOf(LeftEdge(m_run));

      TopEdge(m_run).below(TopEdge(m_main), 6);
      TopEdge(m_cancel).below(TopEdge(m_main), 6);
      TopEdge(m_show_advanced).below(BottomEdge(m_cancel), 6);

      // Size the group frame so that it sits nicely below the buttons.
      TopEdge(m_group_frame).below(BottomEdge(m_show_advanced), 6);
      BottomEdge(m_group_frame).stretchTo(BottomEdge(m_main), -6);
      LeftEdge(m_group_frame).rightOf(LeftEdge(m_main), 6);
      RightEdge(m_group_frame).stretchTo(RightEdge(m_main), -6);
    
      // Layout tab folders.
      for (TabFolderCont::iterator tab_itor = m_tab_folder.begin(); tab_itor != m_tab_folder.end(); ++tab_itor) {
        // Starting height of tab folders is 0.
        long height = 0;
        // Fill a container with this folder's tabs.
        std::map<std::string, IFrame *> tab_cont;
        tab_itor->second->getTabCont(tab_cont);

        // Layout widgets on each tab.
        for (std::map<std::string, IFrame *>::iterator itor = tab_cont.begin(); itor != tab_cont.end(); ++itor) {
          layout(itor->second);
          height = height > itor->second->getHeight() ? height : itor->second->getHeight();
        }
        // Set overall height of the folder to the computed height + the height of the tabs.
        tab_itor->second->getFrame()->setHeight(height + m_tab_height + 10);
      }
    } else {
      std::list<IFrame *> subframes;
      f->getSubframes(subframes);

      std::list<IFrame *>::iterator itor = subframes.begin();
      if (itor != subframes.end()) {
        IFrame * previous = *itor;
        TopEdge(*itor).below(TopEdge(f), 22);
        LeftEdge(*itor).rightOf(LeftEdge(f), 10);
        RightEdge(*itor).stretchTo(RightEdge(f), -10);

        for (++itor; itor != subframes.end(); ++itor) {
          TopEdge(*itor).below(BottomEdge(previous), 6);
          LeftEdge(*itor).rightOf(LeftEdge(f), 10);
          RightEdge(*itor).stretchTo(RightEdge(f), -10);
          previous = *itor;
        }
        BottomEdge(f).stretchTo(BottomEdge(previous), 10);
      }
    }
  }

  void StEventReceiver::run() {
    // Set up standard Gui main window.
    createMainFrame();

    AppParGroup & pars(m_par_group);
    for (hoops::GenParItor itor = pars.begin(); itor != pars.end(); ++itor) {
      const std::string & par_name((*itor)->Name());

      // Changing from GUI to command line mode is not permitted. Also, mode is irrelevant.
      // Skip blank lines as well.
      if (par_name == "gui" || par_name == "mode") continue;
      else if (0 == par_name.size()) continue;

      // Get range of parameter.
      std::list<std::string> par_range;
      bool enumerated_range = parseRange(*itor, par_range);

      // Get the appropriate parent frames for this parameter.
      std::list<IFrame *> parent;
      getParent(*itor, parent);

      // Loop over parents.
      for (std::list<IFrame *>::iterator parent_itor = parent.begin(); parent_itor != parent.end(); ++parent_itor) {
        // If parameter is a switch with an enumerated range, make it a tab-folder.
        if (m_par_group.isSwitch(par_name) && enumerated_range) {
          ITabFolder * tf = m_engine.createTabFolder(*parent_itor, this);
          for (std::list<std::string>::iterator enum_itor = par_range.begin(); enum_itor != par_range.end(); ++enum_itor) {
            tf->addTab(*enum_itor);
          }
          tf->getFrame()->setNaturalSize();
          m_tab_height = tf->getFrame()->getHeight();
          m_tab_folder.insert(std::make_pair(par_name, tf));

          // Record parent of this widget.
          m_parent.insert(std::make_pair(tf->getFrame(), *parent_itor));
        } else {
          // Create standard widget representing each parameter.
          ParWidget * widget = createParWidget(*itor, *parent_itor);

          // Store widget in container.
          m_par_widget.insert(std::make_pair(par_name, widget));

          // Keep track of the widget with the widest label.
          if (0 == m_widest || widget->getLabel()->getWidth() > m_widest->getLabel()->getWidth()) m_widest = widget;

          // Record parent of this widget.
          m_parent.insert(std::make_pair(widget->getFrame(), *parent_itor));
        }
      }
    }

    if (0 != m_widest) {
      m_group_frame->setMinimumWidth(m_widest->getFrame()->getWidth() + 12);
      for (ParWidgetCont::iterator itor = m_par_widget.begin(); itor != m_par_widget.end(); ++itor) {
        itor->second->getLabel()->setWidth(m_widest->getLabel()->getWidth());
      }
    }

    m_engine.run();
  }

  void StEventReceiver::createMainFrame() {
    // Use the name and version of the tool as a label for the GUI window.
    std::string label(m_app->getName());
    if (!label.empty()) label += " ";
    const std::string & version(m_app->getVersion());
    if (!version.empty()) label += "version " + version;

    m_main = m_engine.createMainFrame(this, 650, 600, label);
    m_group_frame = m_engine.createGroupFrame(m_main, this, "Parameters");
    m_run = m_engine.createButton(m_main, this, "text", "Run");
    m_cancel = m_engine.createButton(m_main, this, "text", "Cancel");
    m_show_advanced = m_engine.createButton(m_main, this, "check", "Show Advanced Parameters");

    // Set up some tool tips.
    m_run->setToolTipText("Run the application from inside the GUI");
    m_cancel->setToolTipText("Exit the application and GUI");
    m_show_advanced->setToolTipText("Display advanced (\"hidden\") parameters");

    // Disable prompting.
    AppParGroup & pars(m_par_group);
    pars.suppressPrompts();
  }

  ParWidget * StEventReceiver::createParWidget(hoops::IPar * par, st_graph::IFrame * parent) {
    return new ParWidget(m_engine, parent, par);
  }

  bool StEventReceiver::parseRange(const hoops::IPar * par, std::list<std::string> & range) {
    range.clear();

    const std::string & par_min(par->Min());
    const std::string & par_max(par->Max());

    // Check whether min/max is really min/max or defines a set of enumerated possible values.
    bool enumerated_range = (par_max.empty() && std::string::npos != par_min.find("|"));

    if (!enumerated_range) {
      // min/max are simply min/max.
      range.push_back(par_min);
      range.push_back(par_max);
    } else {
      // Parse enumerated range.
      std::string::const_iterator begin = par_min.begin();
      std::string::const_iterator end = par_min.end();
      while (begin != end) {
        // Skip leading whitespace.
        while (begin != end && isspace(*begin)) ++begin;

        // Move to end of token (end of string or | or whitespace)
        std::string::const_iterator itor = begin;
        for (; itor != end && '|' != *itor && !isspace(*itor); ++itor) {}

        // Save this token in output enumerated range container.
        if (begin != itor) {
          range.push_back(std::string(begin, itor));
        }

        // Skip trailing whitespace.
        for (begin = itor; begin != end && isspace(*begin); ++begin) {}

        // Skip |s.
        while (begin != end && '|' == *begin) ++begin;
      }
    }

    return enumerated_range;
  }

  void StEventReceiver::getParent(const hoops::IPar * par, std::list<st_graph::IFrame *> & parent) {
    const std::string & name(par->Name());
    // Clear out previous container of frames.
    parent.clear();

    // Get cases on which this parameter depends.
    AppParGroup::CaseList case_cont;
    m_par_group.getCase(name, case_cont);

    // Loop over all cases.
    for (AppParGroup::CaseList::iterator itor = case_cont.begin(); itor != case_cont.end(); ++itor) {
      // itor->first == name of switch.
      // itor->second == value of switch.
      // See if switch is displayed as a tab folder.
      TabFolderCont::iterator tab_itor = m_tab_folder.find(itor->first);
      if (m_tab_folder.end() != tab_itor) {
        // Find the tab corresponding to the parameter value given by the second part of the case.
        IFrame * frame = tab_itor->second->getTab(itor->second);
        if (0 != frame) parent.push_back(frame);
      }
    }

    if (parent.empty()) parent.push_back(m_group_frame);
  }

}
