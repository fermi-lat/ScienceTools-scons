/** \file RootFrame.cxx
    \brief Implementation of RootFrame class.
    \author James Peachey, HEASARC/GSSC
*/

#include <stdexcept>

#include "TGFrame.h"

#include "RootEngine.h"
#include "RootFrame.h"

namespace st_graph {

  RootFrame * RootFrame::ancestor() {
    static RootFrame s_ancestor;
    return &s_ancestor;
  }

  RootFrame::RootFrame(RootEngine * engine, IFrame * parent, TGFrame * frame): m_engine(engine), m_parent(0), m_frame(frame) {
    // Make sure the parent is a Root parent.
    m_parent = dynamic_cast<RootFrame *>(parent);
    if (0 == m_parent)
      throw std::logic_error("RootFrame constructor was passed a parent IFrame which is not a RootFrame");

    m_engine->addFrame(this);
  }

  RootFrame::~RootFrame() { if (0 != m_engine) m_engine->removeFrame(this); delete m_frame; }

  void RootFrame::display() {
    if (0 != m_frame) {
      m_frame->MapSubwindows();
      m_frame->Layout();
      m_frame->MapWindow();
    }
  }

  void RootFrame::unDisplay() {
    if (0 != m_frame)
      m_frame->UnmapWindow();
  }

  TGFrame * RootFrame::getTGFrame() { return m_frame; }

  RootFrame::RootFrame(): m_engine(0), m_parent(0), m_frame(0) {}

}
