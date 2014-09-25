/** \file MPLFrame.cxx
    \brief Implementation of MPLFrame class.
    \author Tom Stephens, HEASARC/GSSC
*/
#include <EmbedPython.h>

#include <algorithm>
#include <stdexcept>
#include <iostream>

#include "st_graph/IEventReceiver.h"
#include "st_graph/MPLFrame.h"

//ClassImp(st_graph::MPLFrame)

namespace st_graph {

  MPLFrame * MPLFrame::ancestor() {
    static MPLFrame s_ancestor;
    return &s_ancestor;
  }

  MPLFrame::MPLFrame(IFrame * parent, IEventReceiver * receiver, PyObject * frame, bool delete_parent): m_state(), m_name(),
    m_subframes(), m_parent(0), m_frame(frame), m_receiver(receiver), m_delete_parent(delete_parent), m_minimum_width(0),
    m_minimum_height(0), m_hidden(false) {
    // Make sure the parent is a MPL parent.
    m_parent = dynamic_cast<MPLFrame *>(parent);
    if (0 == m_parent)
      throw std::logic_error("MPLFrame constructor was passed a parent IFrame which is not a MPLFrame");

    m_parent->addFrame(this);
  }

  MPLFrame::MPLFrame(IEventReceiver * receiver, PyObject * frame, bool delete_parent): m_state(), m_name(), m_subframes(),
    m_parent(MPLFrame::ancestor()), m_frame(frame), m_receiver(receiver), m_delete_parent(delete_parent),
    m_minimum_width(0), m_minimum_height(0), m_hidden(false) {
    m_parent->addFrame(this);
  }

  MPLFrame::~MPLFrame() {
    // Note: This appears more complicated than necessary, but be careful changing it. Under some circumstances,
    // a MPLFrame needs to delete its parent, but the parent will always attempt to delete the MPLFrame in the
    // process. Thus it is important to ensure the child is detached at the right times to prevent deleting
    // the parent and/or the child twice.
//	std::cout <<"Called MPLFrame::~MPLFrame()" << std::endl;

    // Save pointer to parent in case delete parent was selected.
    MPLFrame * parent = m_parent;

    // Break all connections between this and its parent.
    if (0 != m_parent) m_parent->removeFrame(this);

    // Delete children.
    while (!m_subframes.empty()) {
      // Find last child.
      std::list<MPLFrame *>::iterator itor = --m_subframes.end();
      // Break links between this and the child.
      removeFrame(*itor);
      // Delete the child.
      delete *itor;
    }

    // Delete the Python frame.
    if(EP_IsType(m_frame,"STTopLevel","STToplevel"))
    	EP_CallMethod(m_frame,"destroy","()");
    Py_DECREF(m_frame);
    m_frame = NULL;

//    if (this == ancestor()){
//    	std::cout << "Destructor called on base object" << std::endl;
//    }

    // In special case where the frame owns its parent frame, delete that as well.
    if (m_delete_parent) delete parent;
  }

  void MPLFrame::display() {
    if (m_hidden) {
      unDisplay();
      return;
    }
    for (std::list<MPLFrame *>::iterator itor = m_subframes.begin(); itor != m_subframes.end(); ++itor)
      (*itor)->display();
    if (0 != m_frame) {
    	bool test = EP_IsType(m_frame,"Tkinter","Tk") || EP_IsType(m_frame,"Tkinter","Toplevel");
    	if (false == test){
    		EP_CallMethod(m_frame,"place","()");
    		EP_CallMethod(m_frame,"pack","()");
    	}
    	EP_CallMethod(m_frame,"update","()");
    }
  }

  void MPLFrame::unDisplay() {
    if (0 != m_frame){
    	bool test = EP_IsType(m_frame,"Tkinter","Tk") || EP_IsType(m_frame,"Tkinter","Toplevel") || m_frame == Py_None;
    	if (false == test){
    		// These are causing failures somewhere
//			EP_CallMethod(m_frame,"pack_forget","()");
//			EP_CallMethod(m_frame,"place_forget","()");
    	}
    }
    for (std::list<MPLFrame *>::reverse_iterator itor = m_subframes.rbegin(); itor != m_subframes.rend(); ++itor)
      (*itor)->unDisplay();
  }

  bool MPLFrame::isHidden() const { return m_hidden; }

  void MPLFrame::setHidden(bool hidden) { m_hidden = hidden; }

  void MPLFrame::addFrame(IFrame * frame) {
    // Make certain this frame is not added to itself.
    if (this == frame) return;

    // Make certain MPL frame is not added more than once.
    if (m_subframes.end() == std::find(m_subframes.begin(), m_subframes.end(), frame)) {
      MPLFrame * f = dynamic_cast<MPLFrame *>(frame);
      if (0 == f) throw std::logic_error("MPLFrame::addFrame was passed an invalid child frame");
//      PyObject *child_f = f->getPythonFrame();

      // This linkage needs to be done then the Python frame is created before it is passed in.
      // It doesn't seem possible to do after the fact.
//      // Check that the contained frame for this object is a Python frame
//      bool correctType = EP_IsType(m_frame,"Tkinter","Frame");
//      // Make this contained frame the parent of the contained frame in the passed in object
//      if (correctType && NULL != child_f) {
//    	  EP_CallMethod(child_f,"__setitem__","(sO)","master",m_frame);
//      }

      // Connect this parent to the child.
      f->m_parent = this;

      // Connect child to this parent.
      m_subframes.push_back(f);
    }
  }

  void MPLFrame::removeFrame(IFrame * frame) {
    std::list<MPLFrame *>::iterator itor = std::find(m_subframes.begin(), m_subframes.end(), frame);
    if (m_subframes.end() != itor) {
      // Disconnect child from parent.
      m_subframes.erase(itor);
      MPLFrame * f = dynamic_cast<MPLFrame *>(frame);
      if (0 == f) throw std::logic_error("MPLFrame::removeFrame was passed an invalid child frame");
//      PyObject * child_f = f->getPythonFrame();

      // Disconnect parent from child.
      f->m_parent = 0;

//      // Check that the contained frame for this object is a Python frame
//      bool correctType = EP_IsType(m_frame,"Tkinter","Frame");
//      // Make this contained frame the parent of the contained frame in the passed in object
//      if (correctType && NULL != child_f) {
//    	  EP_CallMethod(child_f,"__setitem__","(ss)","master","None");
//      }
    }
  }

  void MPLFrame::getSubframes(std::list<IFrame *> & frame_cont) {
    frame_cont.assign(m_subframes.begin(), m_subframes.end());
  }

  const std::string & MPLFrame::getState() const {
    for (bool one_time_only = true; one_time_only ; one_time_only = false) {
      m_state.clear();
        if (EP_IsType(m_frame,"Tkinter","Button")){
			// need to figure out state for a regular Python button.  I don't think it can be done but since
			// I don't think you can read the state while it is actually being pressed, at all other times it is up
			// so we'll just return that.
        	m_state = "up";
        	continue;
        }
        if (EP_IsType(m_frame,"Tkinter","CheckButton")){
        	// in python we have to associate the state with a variable to read it.
        	// not sure how to implement this in the C++ crossover.  Requires a bit of thinking
        	// For now we'll just pass through with an up state
        	m_state = "up";
        	continue;
        }
        if (EP_IsType(m_frame,"Tkinter","Entry")){
        	PyObject *pRes = EP_CallMethod(m_frame,"get","()");
        	char * state;
        	PyArg_Parse(pRes,"(s)",&state);
        	m_state = state;
            continue;
        }
    }
    return m_state;
  }

  void MPLFrame::setState(const std::string & state) {
//    for (bool one_time_only = true; one_time_only ; one_time_only = false) {
//      TGButton * button = dynamic_cast<TGButton *>(m_frame);
//      if (0 != button) {
//        if (state == "down") button->SetState(kButtonDown);
//        else if (state == "up") button->SetState(kButtonUp);
//        // else // TODO issue debug warning.
//        continue;
//      }
//      TGTextEntry * text = dynamic_cast<TGTextEntry *>(m_frame);
//      if (0 != text) {
//        // Only set the text in the entry widget if it has changed. This prevents flicker and moving
//        // the cursor to the end of the line each time a character is typed.
//        std::string current_text = text->GetText();
//        if (current_text != state)
//          text->SetText(state.c_str());
//        continue;
//      }
//    }
  }

  void MPLFrame::setToolTipText(const std::string & text) {
//    // TODO: generalize this to support tool tips for every type of widget.
//    TGButton * button = dynamic_cast<TGButton *>(m_frame);
//    if (0 != button) button->SetToolTipText(text.c_str());
  }

  void MPLFrame::setNaturalSize() {
//    if (0 != m_frame) m_frame->SetSize(m_frame->GetDefaultSize());
  }

  void MPLFrame::layout(bool force_layout) {
//    if (force_layout) setLayoutBroken(m_frame);
//    if (0 != m_receiver) m_receiver->layout(this);
  }

  /// \brief Get the horizontal center of the frame.
  long MPLFrame::getHCenter() const {
    long center = 0;
//    if (0 != m_frame) {
//      // Computation designed to minimize round-off error.
//      long left = m_frame->GetX();
//      // center = left + .5 * width = .5 * (2. * left + width) = (left + left + width) / 2
//      center = (left + left + m_frame->GetWidth()) / 2;
//    }
    return center;
  }

  /// \brief Set the horizontal center of the frame.
  void MPLFrame::setHCenter(long center) {
//    if (0 != m_frame) {
//      // Computation designed to minimize round-off error.
//      long width = m_frame->GetWidth();
//      // left = center - .5 * width = .5 * (2. * center - width) = (center + center - width) / 2
//      long left = (center + center - width) / 2;
//      left = left > 0 ? left : 0;
//      m_frame->Move(left, m_frame->GetY());
//    }
  }

  long MPLFrame::getVCenter() const {
    long center = 0;
//    if (0 != m_frame) {
//      // Computation designed to minimize round-off error.
//      long top = m_frame->GetY();
//      if (0 != m_parent) top += m_parent->getT(); // Convert to absolute coordinates.
//      // center = top + .5 * height = .5 * (2. * top + height) = (top + top + width) / 2
//      center = (top + top + m_frame->GetHeight()) / 2;
//    }
    return center;
  }

  void MPLFrame::setVCenter(long center) {
//    if (0 != m_frame) {
//      // Computation designed to minimize round-off error.
//      long height = m_frame->GetHeight();
//      // top = center - .5 * height = .5 * (2. * center - height) = (center + center - height) / 2
//      long top = (center + center - height) / 2;
//      if (0 != m_parent) top -= m_parent->getT(); // Convert to relative coordinates.
//      top = top > 0 ? top: 0;
//      m_frame->Move(m_frame->GetX(), top);
//    }
  }

  long MPLFrame::getL() const {
    long l = 0;
//    if (0 != m_frame) l = m_frame->GetX();
//    if (0 != m_parent) l += m_parent->getL(); // Convert to absolute coordinates.
    return l;
  }

  void MPLFrame::setL(long l) {
//    if (0 != m_frame) {
//      if (0 != m_parent) l -= m_parent->getL(); // Convert to relative coordinates.
//      l = l > 0 ? l : 0;
//      m_frame->Move(l, m_frame->GetY());
//    }
  }

  long MPLFrame::getR() const {
    long r = 0;
//    if (0 != m_frame) r = m_frame->GetX() + m_frame->GetWidth();
//    if (0 != m_parent) r += m_parent->getL(); // Convert to absolute coordinates.
    return r;
  }

  void MPLFrame::setR(long r) {
//    if (0 != m_frame) {
//      long l = r - m_frame->GetWidth();
//      if (0 != m_parent) l -= m_parent->getL(); // Convert to relative coordinates.
//      l = l > 0 ? l : 0;
//      m_frame->Move(l, m_frame->GetY());
//    }
  }

  long MPLFrame::getT() const {
    long t = 0;
//    if (0 != m_frame) t = m_frame->GetY();
//    if (0 != m_parent) t += m_parent->getT(); // Convert to absolute coordinates.
    return t;
  }

  void MPLFrame::setT(long t) {
//    if (0 != m_frame) {
//      if (0 != m_parent) t -= m_parent->getT(); // Convert to relative coordinates.
//      t = t > 0 ? t : 0;
//      m_frame->Move(m_frame->GetX(), t);
//    }
  }

  long MPLFrame::getB() const {
    long b = 0;
//    if (0 != m_frame) b = m_frame->GetY() + m_frame->GetHeight();
//    if (0 != m_parent) b += m_parent->getT(); // Convert to absolute coordinates.
    return b;
  }

  void MPLFrame::setB(long b) {
//    if (0 != m_frame) {
//      long t = b - m_frame->GetHeight();
//      if (0 != m_parent) t -= m_parent->getT(); // Convert to relative coordinates.
//      t = t > 0 ? t : 0;
//      m_frame->Move(m_frame->GetX(), t);
//    }
  }

  long MPLFrame::getWidth() const {
    long width = 0;
//    if (0 != m_frame) width = m_frame->GetWidth();
    return width;
  }

  void MPLFrame::setWidth(long width) {
    width = (m_minimum_width < width) ? width : m_minimum_width;
    if (0 != m_frame){
    	//@todo Set frame width
    }
  }

  long MPLFrame::getHeight() const {
    long height = 0;
//    if (0 != m_frame) height = m_frame->GetHeight();
    return height;
  }

  void MPLFrame::setHeight(long height) {
    height = (m_minimum_height < height) ? height : m_minimum_height;
    if (0 != m_frame){
    	//@todo Set frame height
    }
  }

  long MPLFrame::getMinimumWidth() const { return m_minimum_width; }

  void MPLFrame::setMinimumWidth(long width) { m_minimum_width = width > 0 ? width : 0; }

  long MPLFrame::getMinimumHeight() const { return m_minimum_height; }

  void MPLFrame::setMinimumHeight(long height) { m_minimum_height = height > 0 ? height : 0; }

//  void MPLFrame::clicked() {
//    if (0 != m_receiver) m_receiver->clicked(this);
//  }
//
//  void MPLFrame::closeWindow() {
//    if (0 != m_receiver) m_receiver->closeWindow(this);
//  }
//
//  void MPLFrame::modified(const char * text) {
//    if (0 != m_receiver) m_receiver->modified(this, text);
//  }

  PyObject * MPLFrame::getPythonFrame() { return m_frame; }

//  void MPLFrame::setTGFrame(TGFrame * frame) { m_frame = frame; }

  IEventReceiver * MPLFrame::getReceiver() { return m_receiver; }

//  void MPLFrame::setLayoutBroken(TGFrame * frame) {
//    frame->SetLayoutBroken(kFALSE);
//    TGCompositeFrame * cf = dynamic_cast<TGCompositeFrame *>(frame);
//    if (0 != cf) {
//      TIter next(cf->GetList());
//      while(TGFrameElement * el = dynamic_cast<TGFrameElement *>(next())) {
//        setLayoutBroken(el->fFrame);
//      }
//    }
//  }

  MPLFrame::MPLFrame(): m_subframes(), m_parent(0), m_frame(0), m_receiver(0) {
	  // Link this to the root Python Tk() frame object
//	  m_frame = EP_CreateObject("Tkinter","Tk","({})","baseName","st_graph.app");
//	  m_frame = EP_CreateObject("Tkinter","Tk","(ss)","st_graph","st_graph");
	  m_frame = EP_CreateObject("Tkinter","Tk","()");
	  EP_CallMethod(m_frame,"withdraw","()");
//	  m_frame = EP_CreateObject("Tkinter","Toplevel","()");
//	  m_frame = Py_None;

  }
//  MPLFrame::MPLFrame() {}

}
