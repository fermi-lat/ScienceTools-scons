/** \file RootTabFolder.cxx
    \brief Implementation for RootTabFolder class.
    \author James Peachey, HEASARC/GSSC
*/
#include <utility>

#include "TGFrame.h"
#include "TGTab.h"

#include "RootTabFolder.h"
#include "STGLayoutManager.h"

#include "st_graph/Engine.h"
#include "st_graph/RootFrame.h"

namespace st_graph {

  RootTabFolder::RootTabFolder(RootFrame * parent, IEventReceiver * receiver): m_tab(), m_frame(0), m_tg_tab(0),
    m_receiver(receiver) {
    TGFrame * tg_parent = parent->getTGFrame();
    m_tg_tab = new TGTab(tg_parent, 10, 10);

    m_frame = new RootFrame(parent, 0, m_tg_tab);
    m_frame->setName("tab folder");
  }

  RootTabFolder::~RootTabFolder() {}

  IFrame * RootTabFolder::addTab(const std::string & label) {
    IFrame * frame = getTab(label);

    if (0 == frame) {
      // Use AddTab to create a new tab area. This connects the Root TGFrame objects correctly.
      TGCompositeFrame * tab = m_tg_tab->AddTab(label.c_str());

      // Create a RootFrame with NULL TGFrame pointer, to prevent calling parent->addFrame().
      // The tab above will be assigned to the RootFrame below.
      RootFrame * root_frame = new RootFrame(m_frame, m_receiver, 0);

      root_frame->setName("tab " + label);

      // Give tab_frame possession of the Root TGTab widget. This simply assigns the pointer.
      root_frame->setTGFrame(tab);

      // Have composite frame use the event receiver for all layouts.
      if (0 != m_receiver) tab->SetLayoutManager(new STGLayoutManager(m_receiver, root_frame, tab));

      // Keep track of tabs so that they can be obtained again later.
      m_tab.insert(std::make_pair(label, root_frame));

      frame = root_frame;
    }

    return frame;
  }

  IFrame * RootTabFolder::getTab(const std::string & label) {
    IFrame * frame = 0;
    std::map<std::string, IFrame *>::iterator itor = m_tab.find(label);
    if (m_tab.end() != itor) frame = itor->second;
    return frame;
  }

  IFrame * RootTabFolder::getFrame() { return m_frame; }

  void RootTabFolder::select(IFrame *) {
  }

  void RootTabFolder::getTabCont(std::map<std::string, IFrame *> & tab_cont) {
    tab_cont = m_tab;
  }

}
