/** \file MPLEngine.cxx
    \brief Implementation of class which encapsulates the matplotlib graphics implementation.
    \author Tom Stephens, HEASARC/GSSC
*/
#include <EmbedPython.h>

//#include <cctype>
//#include <cstdlib>
//#include <csignal>
#include <cstring>
#include <stdexcept>
//#include <vector>

//typedef void (*root_signal_handler_t) (int);

#include "st_graph/IEventReceiver.h"
#include "st_graph/IFrame.h"
#include "st_graph/MPLFrame.h"
//#include "st_app/StApp.h"

#include "MPLEngine.h"
#include "MPLPlot.h"
#include "MPLPlotFrame.h"
#include "MPLTabFolder.h"
//#include "STGLayoutManager.h"
//#include "STGMainFrame.h"

namespace {

  // Receiver which terminates the application -- sensible default behavior for a main frame.
  class DefaultReceiver : public st_graph::IEventReceiver {
    public:
      DefaultReceiver(): m_engine(st_graph::Engine::instance()), m_exit_on_close(true) {}

      // General behavior when closing window is to close the matplotlib window.
      virtual void closeWindow(st_graph::IFrame * f) {
//        st_graph::MPLFrame * rf = dynamic_cast<st_graph::MPLFrame *>(f);
//        if (0 != rf) {
//          TGFrame * tgf = rf->getTGFrame();
//          if (0 != tgf) tgf->UnmapWindow();
//        }
//        if (m_exit_on_close) m_engine.stop();
      }

      void setExitOnClose(bool exit_on_close) { m_exit_on_close = exit_on_close; }

    private:
      st_graph::Engine & m_engine;
      bool m_exit_on_close;
  };

  DefaultReceiver * s_default_receiver = 0;

}

namespace st_graph {

  MPLEngine::MPLEngine(): m_init_succeeded(false) {
	  // Let's get Python initialized
	  Py_Initialize();
      m_init_succeeded = true;
  }

  void MPLEngine::run() {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::run: graphical environment not initialized");

    // Display all frames currently linked to the top-level frame.
    MPLFrame::ancestor()->display();

    // Hide all frames which need to be hidden at the outset.
    hideHidden(MPLFrame::ancestor());

    // Fire off the ancestor object's mainloop() to handle the graphical displays.
    //@todo  This needs to change to fire off all the subframe mainloop() methods instead of the top Tk object's method
    EP_CallMethod(MPLFrame::ancestor()->getPythonFrame(),"mainloop","()");
  }

  void MPLEngine::stop() {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::stop: graphical environment not initialized");

    // Hide all frames currently linked to the top-level frame.
    MPLFrame::ancestor()->unDisplay();

    //@todo  This needs to change to fire off all the subframe destroy methods instead of the top Tk object's method
    EP_CallMethod(MPLFrame::ancestor()->getPythonFrame(),"destroy","()");
  }

  IFrame * MPLEngine::createMainFrame(IEventReceiver * receiver, unsigned int width, unsigned int height,
    const std::string & title) {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createMainFrame: graphical environment not initialized");

    // If client did not supply a receiver, use the default one.
    if (0 == receiver) {
      if (0 == s_default_receiver) s_default_receiver = new DefaultReceiver;
      receiver = s_default_receiver;
    }

    // Create the main Python frame.
    // Note:  Due to the way Python widgets are created, their parent has to be declared at time of creation
    //        Since this is a main frame, we're linking it to the ancestor object
//    PyObject * p_widget = EP_CreateObject("Tkinter","Toplevel","(O)", MPLFrame::ancestor()->getPythonFrame());
    PyObject * p_widget = EP_CreateObject("STTopLevel","STToplevel","(O)", MPLFrame::ancestor()->getPythonFrame());
    EP_CallMethod(p_widget,"__setitem__","(si)","width",width);
    EP_CallMethod(p_widget,"__setitem__","(si)","height",height);
//    PyObject *pfunc = EP_GetMethod(p_widget,"quit");  // get the function that stops the mainloop() for this window
    PyObject *pfunc = EP_GetMethod(p_widget,"stop");  // get the function that stops the mainloop() for this window
    EP_CallMethod(p_widget,"protocol","(sO)","WM_DELETE_WINDOW", pfunc);  // assign that function as the callback for closing the window.

    // To set the main frame title we actually set the title of the ancestor window
    EP_CallMethod(p_widget,"title","(s)",title.c_str());

    // Give window manager a hint about where to display it. (again this get's applied to the ancestor object)
    EP_CallMethod(p_widget,"geometry","(s)","+100+50");

    // Create the IFrame which refers to it.
    MPLFrame * frame = new MPLFrame(receiver, p_widget);

    frame->setName("main");
    return frame;
  }

  IPlot * MPLEngine::createPlot(const std::string & title, unsigned int width, unsigned int height, const std::string & style,
    const ISequence & x, const ISequence & y) {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createPlot: graphical environment not initialized");

    // Create parent main frame.
    IFrame * mf = createMainFrame(0, width, height);

    // Create frame to hold plot. This frame owns and will delete its parent.
    IFrame * pf = new MPLPlotFrame(mf, title, width, height, true);

    // Create plot. This plot owns and will delete its parent.
    return new MPLPlot(pf, style, x, y, true);
  }

  IPlot * MPLEngine::createPlot(const std::string & title, unsigned int width, unsigned int height, const std::string & style,
    const ISequence & x, const ISequence & y, const std::vector<std::vector<double> > & z) {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createPlot: graphical environment not initialized");

    // Create parent main frame.
    IFrame * mf = createMainFrame(0, width, height);

    // Create frame to hold plot. This frame owns and will delete its parent.
    IFrame * pf = new MPLPlotFrame(mf, title, width, height, true);

    // Create plot. This plot owns and will delete its parent.
    return new MPLPlot(pf, style, x, y, z, true);
  }

  IPlot * MPLEngine::createPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y) {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createPlot: graphical environment not initialized");

    return new MPLPlot(parent, style, x, y);
  }

  IPlot * MPLEngine::createPlot(IFrame * parent, const std::string & style, const ISequence & x, const ISequence & y,
    const std::vector<std::vector<double> > & z) {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createPlot: graphical environment not initialized");

    return new MPLPlot(parent, style, x, y, z);
  }

  IFrame * MPLEngine::createPlotFrame(IFrame * parent, const std::string & title, unsigned int width, unsigned int height) {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createPlotFrame: graphical environment not initialized");

    return new MPLPlotFrame(parent, title, width, height);
  }

  IFrame * MPLEngine::createButton(IFrame * parent, IEventReceiver * receiver, const std::string & style,
    const std::string & label) {
	  // this is used for the GUI functionality which is not implemented in they Python version so let the user know
	  throw std::runtime_error("GUI capabilities not implemented in this version of the Science Tools");
//    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createButton: graphical environment not initialized");
//
//    // Need the Root frame of the parent object.
//    MPLFrame * rf = dynamic_cast<MPLFrame *>(parent);
//    if (0 == rf) throw std::logic_error("MPLEngine::createButton was passed an invalid parent frame pointer");
//
//    // Make style check case insensitive.
//    std::string lc_style = style;
//    for (std::string::iterator itor = lc_style.begin(); itor != lc_style.end(); ++itor) *itor = tolower(*itor);
//
//    TGButton * tg_widget = 0;
//    if (std::string::npos != lc_style.find("check")) {
//      // Create the Root widget.
//      tg_widget = new TGCheckButton(rf->getTGFrame(), label.c_str());
//    } else if (std::string::npos != lc_style.find("text")) {
//      // Create the Root widget.
//      tg_widget = new TGTextButton(rf->getTGFrame(), label.c_str());
//    } else {
//      throw std::logic_error("MPLEngine::createButton cannot create a button with style " + style);
//    }
//
//    // Create the IFrame which refers to it.
//    IFrame * frame = new MPLFrame(parent, receiver, tg_widget);
//
//    frame->setName("button " + label);
//
//    // Connect appropriate Root Qt signals to this object's slot.
//    tg_widget->Connect("Clicked()", "st_graph::MPLFrame", frame, "clicked()");
//
    return NULL;//frame;
  }

  IFrame * MPLEngine::createLabel(IFrame * parent, IEventReceiver * receiver, const std::string & text) {
	  // this is used for the GUI functionality which is not implemented in they Python version so let the user know
	  throw std::runtime_error("GUI capabilities not implemented in this version of the Science Tools");
//    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createLabel: graphical environment not initialized");
//
//    // Need the Root frame of the parent object.
//    MPLFrame * rf = dynamic_cast<MPLFrame *>(parent);
//    if (0 == rf) throw std::logic_error("MPLEngine::createLabel was passed an invalid parent frame pointer");
//
//    TGLabel * tg_widget  = new TGLabel(rf->getTGFrame(), text.c_str());
//
//    tg_widget->SetTextJustify(kTextRight);
//
//    IFrame * frame = new MPLFrame(parent, receiver, tg_widget);
//
//    frame->setName("label " + text);
//
    return NULL;//frame;
  }

  IFrame * MPLEngine::createTextEntry(IFrame * parent, IEventReceiver * receiver, const std::string & content) {
	  // this is used for the GUI functionality which is not implemented in they Python version so let the user know
	  throw std::runtime_error("GUI capabilities not implemented in this version of the Science Tools");
//    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createTextEntry: graphical environment not initialized");
//
//    // Need the Root frame of the parent object.
//    MPLFrame * rf = dynamic_cast<MPLFrame *>(parent);
//    if (0 == rf) throw std::logic_error("MPLEngine::createTextEntry was passed an invalid parent frame pointer");
//
//    TGTextEntry * tg_widget  = new TGTextEntry(rf->getTGFrame(), content.c_str());
//
//    IFrame * frame = new MPLFrame(parent, receiver, tg_widget);
//
//    frame->setName("text entry " + content);
//
//    // Whenever text is changed, send the changes back to root frame.
//    tg_widget->Connect("TextChanged(char *)", "st_graph::MPLFrame", frame, "modified(const char *)");
//
    return NULL;// frame;
  }

  IFrame * MPLEngine::createComposite(IFrame * parent, IEventReceiver * receiver) {
	  // this is used for the GUI functionality which is not implemented in they Python version so let the user know
	  throw std::runtime_error("GUI capabilities not implemented in this version of the Science Tools");
//    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createComposite: graphical environment not initialized");
//
//    // Need the Root frame of the parent object.
//    MPLFrame * rf = dynamic_cast<MPLFrame *>(parent);
//    if (0 == rf) throw std::logic_error("MPLEngine::createComposite was passed an invalid parent frame pointer");
//
//    TGCompositeFrame * tg_widget  = new TGCompositeFrame(rf->getTGFrame(), 20, 20);
//
//    MPLFrame * frame = new MPLFrame(parent, receiver, tg_widget);
//
//    frame->setName("composite frame inside " + parent->getName());
//
//    // Create a layout manager for the Root widget which uses the receiver to manage the layout.
//    tg_widget->SetLayoutManager(new STGLayoutManager(receiver, frame, tg_widget));
//
    return NULL;// frame;
  }

  IFrame * MPLEngine::createGroupFrame(IFrame * parent, IEventReceiver * receiver, const std::string & label) {
	  // this is used for the GUI functionality which is not implemented in they Python version so let the user know
	  throw std::runtime_error("GUI capabilities not implemented in this version of the Science Tools");
//    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createGroupFrame: graphical environment not initialized");
//
//    // Need the Root frame of the parent object.
//    MPLFrame * rf = dynamic_cast<MPLFrame *>(parent);
//    if (0 == rf) throw std::logic_error("MPLEngine::createGroupFrame was passed an invalid parent frame pointer");
//
//    TGGroupFrame * tg_widget  = new TGGroupFrame(rf->getTGFrame(), label.c_str());
//
//    MPLFrame * frame = new MPLFrame(parent, receiver, tg_widget);
//
//    frame->setName("group frame " + label);
//
//    // Create a layout manager for the Root widget which uses the receiver to manage the layout.
//    tg_widget->SetLayoutManager(new STGLayoutManager(receiver, frame, tg_widget));
//
    return NULL;//frame;
  }

  ITabFolder * MPLEngine::createTabFolder(IFrame * parent, IEventReceiver * receiver) {
	  // this is used for the GUI functionality which is not implemented in they Python version so let the user know
	  throw std::runtime_error("GUI capabilities not implemented in this version of the Science Tools");
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::createTabFolder: graphical environment not initialized");

    // Need the Root frame of the parent object.
    MPLFrame * rf = dynamic_cast<MPLFrame *>(parent);
    if (0 == rf) throw std::logic_error("MPLEngine::createTabFolder was passed an invalid parent frame pointer");

    MPLTabFolder * folder = new MPLTabFolder(rf, receiver);

    return folder;
  }

  std::string MPLEngine::fileDialog(IFrame *, const std::string & initial_file_name, const std::string & style) {
    if (!m_init_succeeded) throw std::runtime_error("MPLEngine::fileDialog: graphical environment not initialized");
	  // this is used for the GUI functionality which is not implemented in they Python version so let the user know
	  throw std::runtime_error("GUI capabilities not implemented in this version of the Science Tools");

//    std::string dir;
//    std::string file;
//
//    // Split input file name into directory and file.
//    std::string::size_type delim_pos = initial_file_name.find_last_of("/\\");
//    if (std::string::npos == delim_pos) {
//      // No delimiter: assume directory only was supplied.
//      dir = initial_file_name;
//    } else {
//      // Delimiter found: split into before/after delimiter.
//      dir = initial_file_name.substr(0, delim_pos);
//      file = initial_file_name.substr(delim_pos + 1, std::string::npos);
//    }
//
//    // Create structure to hold the details.
//    TGFileInfo * info = new TGFileInfo;
//
//    // Set the widget's initial directory.
//    info->fIniDir = new char[dir.size() + 1];
//    strcpy(info->fIniDir, dir.c_str());
//
//    // Set the widget's initial file name.
//    char * buf = new char[file.size() + 1];
//    info->fFilename = buf;
//    strcpy(info->fFilename, file.c_str());
//
//    // Set up style.
//    EFileDialogMode mode = kFDOpen;
//    if (style != "open") mode = kFDSave;
//
//    // Create the dialog box. This will run the inner event loop until user closes the dialog box one way or another.
//    // At that point the dialog box will be automatically deleted.
//    new TGFileDialog(gClient->GetRoot(), gClient->GetRoot(), mode, info);
//
//    // Get the file name.
    std::string file_name = initial_file_name;
//    if (0 != info->fFilename) file_name = info->fFilename;
//
//    // Clean up.
//    delete info;
//    delete [] buf;
//
    return file_name;
  }

  void MPLEngine::setDefaultExitOnClose(bool exit_on_close) {
    if (0 == s_default_receiver) s_default_receiver = new DefaultReceiver;
    s_default_receiver->setExitOnClose(exit_on_close);
  }

  void MPLEngine::hideHidden(IFrame * frame) {
    std::list<IFrame *> subframes;
    frame->getSubframes(subframes);
    for (std::list<IFrame *>::iterator itor = subframes.begin(); itor != subframes.end(); ++itor) {
      hideHidden(*itor);
    }
    if (frame->isHidden()) frame->unDisplay(); // Will hide all children too.
  }

}
