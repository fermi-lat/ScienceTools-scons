/** \file RootTabFolder.cxx
    \brief Implementation for RootTabFolder class.
    \author James Peachey, HEASARC/GSSC
*/

#include "TGFrame.h"
#include "TGTab.h"

#include "RootTabFolder.h"

#include "st_graph/Engine.h"
#include "st_graph/RootFrame.h"

namespace st_graph {

  RootTabFolder::RootTabFolder(RootFrame * parent, IEventReceiver * receiver): m_frame(0), m_tg_tab(0), m_receiver(receiver) {
    TGFrame * tg_parent = parent->getTGFrame();
    m_tg_tab = new TGTab(tg_parent, 10, 10);

    m_frame = new RootFrame(parent, 0, m_tg_tab);
  }

  IFrame * RootTabFolder::addTab(const std::string & label) {
    // Use AddTab to create a new tab area. This connects the Root TGFrame objects correctly.
    TGCompositeFrame * tab = m_tg_tab->AddTab(label.c_str());

    // Create a RootFrame with NULL TGFrame pointer, to prevent calling parent->addFrame().
    RootFrame * tab_frame = new RootFrame(m_frame, 0, 0);

    // Give tab_frame possession of the Root TGTab widget.
    tab_frame->setTGFrame(tab);

    return tab_frame;
  }

  IFrame * RootTabFolder::getFrame() { return m_frame; }

  void RootTabFolder::select(IFrame *) {
  }

}
