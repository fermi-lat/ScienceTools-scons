/** \file RootFrame.cxx
    \brief Implementation of RootFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#include <algorithm>
#include <stdexcept>

#include "TGButton.h"
#include "TGFrame.h"
#include "TGLayout.h"
#include "TGTextEntry.h"

#include "st_graph/IEventReceiver.h"
#include "st_graph/RootFrame.h"

ClassImp(st_graph::RootFrame)

namespace st_graph {

  RootFrame * RootFrame::ancestor() {
    static RootFrame s_ancestor;
    return &s_ancestor;
  }

  RootFrame::RootFrame(IFrame * parent, IEventReceiver * receiver, TGFrame * frame, bool delete_parent): m_state(), m_subframes(),
    m_parent(0), m_frame(frame), m_receiver(receiver), m_delete_parent(delete_parent), m_minimum_width(0), m_minimum_height(0) {
    // Make sure the parent is a Root parent.
    m_parent = dynamic_cast<RootFrame *>(parent);
    if (0 == m_parent)
      throw std::logic_error("RootFrame constructor was passed a parent IFrame which is not a RootFrame");

    m_parent->addFrame(this);
  }

  RootFrame::RootFrame(IEventReceiver * receiver, TGFrame * frame, bool delete_parent): m_state(), m_subframes(),
    m_parent(RootFrame::ancestor()), m_frame(frame), m_receiver(receiver), m_delete_parent(delete_parent),
    m_minimum_width(0), m_minimum_height(0) {
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
      std::list<RootFrame *>::iterator itor = --m_subframes.end();
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
    if (0 != m_frame) {
      m_frame->MapSubwindows();
      m_frame->Layout();
      m_frame->MapWindow();
    }
    for (std::list<RootFrame *>::iterator itor = m_subframes.begin(); itor != m_subframes.end(); ++itor)
      (*itor)->display();
  }

  void RootFrame::unDisplay() {
    if (0 != m_frame)
      m_frame->UnmapWindow();
    for (std::list<RootFrame *>::reverse_iterator itor = m_subframes.rbegin(); itor != m_subframes.rend(); ++itor)
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
    std::list<RootFrame *>::iterator itor = std::find(m_subframes.begin(), m_subframes.end(), frame);
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

  void RootFrame::getSubframes(std::list<IFrame *> & frame_cont) {
    frame_cont.assign(m_subframes.begin(), m_subframes.end());
  }

  const std::string & RootFrame::getState() const {
    for (bool one_time_only = true; one_time_only ; one_time_only = false) {
      m_state.clear();
      const TGButton * button = dynamic_cast<const TGButton *>(m_frame);
      if (0 != button) {
        EButtonState state = button->GetState();
        if (kButtonDown == state) m_state = "down";
        else if (kButtonUp == state) m_state = "up";
        // else // TODO issue debug warning.
        continue;
      }
      const TGTextEntry * text = dynamic_cast<const TGTextEntry *>(m_frame);
      if (0 != text) {
        m_state = text->GetText();
        continue;
      }
    }
    return m_state;
  }

  void RootFrame::setState(const std::string & state) {
    for (bool one_time_only = true; one_time_only ; one_time_only = false) {
      TGButton * button = dynamic_cast<TGButton *>(m_frame);
      if (0 != button) {
        if (state == "down") button->SetState(kButtonDown);
        else if (state == "up") button->SetState(kButtonUp);
        // else // TODO issue debug warning.
        continue;
      }
      TGTextEntry * text = dynamic_cast<TGTextEntry *>(m_frame);
      if (0 != text) {
        text->SetText(state.c_str());
        continue;
      }
    }
  }

  void RootFrame::setToolTipText(const std::string & text) {
    // TODO: generalize this to support tool tips for every type of widget.
    TGButton * button = dynamic_cast<TGButton *>(m_frame);
    if (0 != button) button->SetToolTipText(text.c_str());
  }

  void RootFrame::setNaturalSize() {
    if (0 != m_frame) m_frame->SetSize(m_frame->GetDefaultSize());
  }

  /// \brief Get the horizontal center of the frame.
  long RootFrame::getHCenter() const {
    long center = 0;
    if (0 != m_frame) {
      // Computation designed to minimize round-off error.
      long left = m_frame->GetX();
      // center = left + .5 * width = .5 * (2. * left + width) = (left + left + width) / 2
      center = (left + left + m_frame->GetWidth()) / 2;
    }
    return center;
  }

  /// \brief Set the horizontal center of the frame.
  void RootFrame::setHCenter(long center) {
    if (0 != m_frame) {
      // Computation designed to minimize round-off error.
      long width = m_frame->GetWidth();
      // left = center - .5 * width = .5 * (2. * center - width) = (center + center - width) / 2
      long left = (center + center - width) / 2;
      left = left > 0 ? left : 0;
      m_frame->Move(left, m_frame->GetY());
    }
  }

  long RootFrame::getVCenter() const {
    long center = 0;
    if (0 != m_frame) {
      // Computation designed to minimize round-off error.
      long top = m_frame->GetY();
      if (0 != m_parent) top += m_parent->getT(); // Convert to absolute coordinates.
      // center = top + .5 * height = .5 * (2. * top + height) = (top + top + width) / 2
      center = (top + top + m_frame->GetHeight()) / 2;
    }
    return center;
  }

  void RootFrame::setVCenter(long center) {
    if (0 != m_frame) {
      // Computation designed to minimize round-off error.
      long height = m_frame->GetHeight();
      // top = center - .5 * height = .5 * (2. * center - height) = (center + center - height) / 2
      long top = (center + center - height) / 2;
      if (0 != m_parent) top -= m_parent->getT(); // Convert to relative coordinates.
      top = top > 0 ? top: 0;
      m_frame->Move(m_frame->GetX(), top);
    }
  }

  long RootFrame::getL() const {
    long l = 0;
    if (0 != m_frame) l = m_frame->GetX();
    if (0 != m_parent) l += m_parent->getL(); // Convert to absolute coordinates.
    return l;
  }

  void RootFrame::setL(long l) {
    if (0 != m_frame) {
      if (0 != m_parent) l -= m_parent->getL(); // Convert to relative coordinates.
      l = l > 0 ? l : 0;
      m_frame->Move(l, m_frame->GetY());
    }
  }

  long RootFrame::getR() const {
    long r = 0;
    if (0 != m_frame) r = m_frame->GetX() + m_frame->GetWidth();
    if (0 != m_parent) r += m_parent->getL(); // Convert to absolute coordinates.
    return r;
  }

  void RootFrame::setR(long r) {
    if (0 != m_frame) {
      long l = r - m_frame->GetWidth();
      if (0 != m_parent) l -= m_parent->getL(); // Convert to relative coordinates.
      l = l > 0 ? l : 0;
      m_frame->Move(l, m_frame->GetY());
    }
  }

  long RootFrame::getT() const {
    long t = 0;
    if (0 != m_frame) t = m_frame->GetY();
    if (0 != m_parent) t += m_parent->getT(); // Convert to absolute coordinates.
    return t;
  }

  void RootFrame::setT(long t) {
    if (0 != m_frame) {
      if (0 != m_parent) t -= m_parent->getT(); // Convert to relative coordinates.
      t = t > 0 ? t : 0;
      m_frame->Move(m_frame->GetX(), t);
    }
  }

  long RootFrame::getB() const {
    long b = 0;
    if (0 != m_frame) b = m_frame->GetY() + m_frame->GetHeight();
    if (0 != m_parent) b += m_parent->getT(); // Convert to absolute coordinates.
    return b;
  }

  void RootFrame::setB(long b) {
    if (0 != m_frame) {
      long t = b - m_frame->GetHeight();
      if (0 != m_parent) t -= m_parent->getT(); // Convert to relative coordinates.
      t = t > 0 ? t : 0;
      m_frame->Move(m_frame->GetX(), t);
    }
  }

  long RootFrame::getWidth() const {
    long width = 0;
    if (0 != m_frame) width = m_frame->GetWidth();
    return width;
  }

  void RootFrame::setWidth(long width) {
    width = (m_minimum_width < width) ? width : m_minimum_width;
    if (0 != m_frame)
      m_frame->SetWidth(width);
  }

  long RootFrame::getHeight() const {
    long height = 0;
    if (0 != m_frame) height = m_frame->GetHeight();
    return height;
  }

  void RootFrame::setHeight(long height) {
    height = (m_minimum_height < height) ? height : m_minimum_height;
    if (0 != m_frame)
      m_frame->SetHeight(height);
  }

  long RootFrame::getMinimumWidth() const { return m_minimum_width; }

  void RootFrame::setMinimumWidth(long width) { m_minimum_width = width > 0 ? width : 0; }

  long RootFrame::getMinimumHeight() const { return m_minimum_height; }

  void RootFrame::setMinimumHeight(long height) { m_minimum_height = height > 0 ? height : 0; }

  void RootFrame::clicked() {
    if (0 != m_receiver) m_receiver->clicked(this);
  }

  void RootFrame::closeWindow() {
    if (0 != m_receiver) m_receiver->closeWindow(this);
  }

  void RootFrame::modified(const char * text) {
    if (0 != m_receiver) m_receiver->modified(this, text);
  }

  TGFrame * RootFrame::getTGFrame() { return m_frame; }

  void RootFrame::setTGFrame(TGFrame * frame) { m_frame = frame; }

  RootFrame::RootFrame(): m_subframes(), m_parent(0), m_frame(0), m_receiver(0) {}

}
