/** \file RootFrame.cxx
    \brief Implementation of RootFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#include <algorithm>
#include <stdexcept>

#include "TGFrame.h"

#include "st_graph/IEventReceiver.h"

#include "RootFrame.h"

ClassImp(st_graph::RootFrame)

namespace st_graph {

  RootFrame * RootFrame::ancestor() {
    static RootFrame s_ancestor;
    return &s_ancestor;
  }

  RootFrame::RootFrame(IFrame * parent, IEventReceiver * receiver, TGFrame * frame, bool delete_parent): m_subframes(),
    m_parent(0), m_frame(frame), m_receiver(receiver), m_delete_parent(delete_parent) {
    // Make sure the parent is a Root parent.
    m_parent = dynamic_cast<RootFrame *>(parent);
    if (0 == m_parent)
      throw std::logic_error("RootFrame constructor was passed a parent IFrame which is not a RootFrame");

    m_parent->addFrame(this);
  }

  RootFrame::RootFrame(IEventReceiver * receiver, TGFrame * frame, bool delete_parent): m_subframes(),
    m_parent(RootFrame::ancestor()), m_frame(frame), m_receiver(receiver), m_delete_parent(delete_parent) {
    m_parent->addFrame(this);
  }

  RootFrame::~RootFrame() {
    // Note: This appears more complicated than necessary, but be careful changing it. Under some circumstances,
    // a RootFrame needs to delete its parent, but the parent will always attempt to delete the RootFrame in the
    // process. Thus it is important to ensure the child is detached at the right times to prevent deleting
    // the parent and/or the child twice.

    // Save pointer to parent in case delete parent was selected.
    RootFrame * parent = m_parent;

    // Break all connections between this and its parent.
    if (0 != m_parent) m_parent->removeFrame(this);

    // Delete children.
    while (!m_subframes.empty()) {
      // Find last child.
      std::list<IFrame *>::iterator itor = --m_subframes.end();
      // Break links between this and the child.
      removeFrame(*itor);
      // Delete the child.
      delete *itor;
    }

    // Delete the root frame.
    delete m_frame;

    // In special case where the frame owns its parent frame, delete that as well.
    if (m_delete_parent) delete parent;
  }

  void RootFrame::display() {
    for (std::list<IFrame *>::iterator itor = m_subframes.begin(); itor != m_subframes.end(); ++itor)
      (*itor)->display();
    if (0 != m_frame) {
      m_frame->MapSubwindows();
      m_frame->Layout();
      m_frame->MapWindow();
    }
  }

  void RootFrame::unDisplay() {
    if (0 != m_frame)
      m_frame->UnmapWindow();
    for (std::list<IFrame *>::reverse_iterator itor = m_subframes.rbegin(); itor != m_subframes.rend(); ++itor)
      (*itor)->unDisplay();
  }

  void RootFrame::addFrame(IFrame * frame) {
    // Make certain Root frame is not added more than once.
    if (m_subframes.end() == std::find(m_subframes.begin(), m_subframes.end(), frame)) {
      RootFrame * rf = dynamic_cast<RootFrame *>(frame);
      if (0 == rf) throw std::logic_error("RootFrame::addFrame was passed an invalid child frame");
      TGFrame * child_tg_f = rf->getTGFrame();

      TGCompositeFrame * parent_tg_f = dynamic_cast<TGCompositeFrame *>(m_frame);
      if (0 != parent_tg_f && 0 != child_tg_f) parent_tg_f->AddFrame(child_tg_f);

      // Connect this parent to the child.
      rf->m_parent = this;

      // Connect child to this parent.
      m_subframes.push_back(rf);
    }
  }

  void RootFrame::removeFrame(IFrame * frame) {
    std::list<IFrame *>::iterator itor = std::find(m_subframes.begin(), m_subframes.end(), frame);
    if (m_subframes.end() != itor) {
      // Disconnect child from parent.
      m_subframes.erase(itor);
      RootFrame * rf = dynamic_cast<RootFrame *>(frame);
      if (0 == rf) throw std::logic_error("RootFrame::removeFrame was passed an invalid child frame");
      TGFrame * child_tg_f = rf->getTGFrame();

      // Disconnect parent from child.
      rf->m_parent = 0;

      TGCompositeFrame * parent_tg_f = dynamic_cast<TGCompositeFrame *>(m_frame);
      if (0 != parent_tg_f && 0 != child_tg_f) parent_tg_f->RemoveFrame(child_tg_f);
    }
  }

  long RootFrame::getL() const {
    long l = 0;
    if (0 != m_frame) l = m_frame->GetX();
    return l;
  }

  void RootFrame::setL(long l) { if (0 != m_frame) m_frame->Move(l, l + m_frame->GetWidth()); }

  long RootFrame::getR() const {
    long r = 0;
    if (0 != m_frame) r = m_frame->GetX() + m_frame->GetWidth();
    return r;
  }

  void RootFrame::setR(long r) {
    if (0 != m_frame) {
      r -= m_frame->GetX();
      if (r >= 0) m_frame->SetWidth(r);
    }
  }

  void RootFrame::clicked() {
    m_receiver->clicked(this);
  }

  void RootFrame::closeWindow() {
    m_receiver->closeWindow(this);
  }

  TGFrame * RootFrame::getTGFrame() { return m_frame; }

  RootFrame::RootFrame(): m_subframes(), m_parent(0), m_frame(0), m_receiver(0) {}

}
