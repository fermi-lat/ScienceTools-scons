/** \file RootFrame.cxx
    \brief Implementation of RootFrame class.
    \author James Peachey, HEASARC/GSSC
*/

#include <stdexcept>

#include "TGFrame.h"

#include "RootFrame.h"

namespace st_graph {

  RootFrame * RootFrame::ancestor() {
    static RootFrame s_ancestor;
    return &s_ancestor;
  }

  RootFrame::RootFrame(IFrame * parent, TGFrame * frame): m_subframes(), m_parent(0),
    m_frame(frame) {
    // Make sure the parent is a Root parent.
    m_parent = dynamic_cast<RootFrame *>(parent);
    if (0 == m_parent)
      throw std::logic_error("RootFrame constructor was passed a parent IFrame which is not a RootFrame");

    m_parent->addFrame(this);
  }

  RootFrame::~RootFrame() {
    if (0 != m_parent) m_parent->removeFrame(this);
    delete m_frame;
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
    m_subframes.push_back(frame);
  }

  void RootFrame::removeFrame(IFrame * frame) {
    m_subframes.remove(frame);
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

  TGFrame * RootFrame::getTGFrame() { return m_frame; }

  RootFrame::RootFrame(): m_subframes(), m_parent(0), m_frame(0) {}

}
