/** \file RootFrame.h
    \brief Interface for RootFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootFrame_h
#define st_graph_RootFrame_h

#include <list>

#include "RQ_OBJECT.h"
#include "Rtypes.h"
#include "TQObject.h"

#include "st_graph/IFrame.h"

class TGFrame;

namespace st_graph {

  class IEventReceiver;

  /** \class RootFrame
      \brief Interface for base class frame for all graphical frames.
  */
  class RootFrame : public IFrame {
      RQ_OBJECT("st_graph::RootFrame")
      ClassDef(RootFrame, 0)

    public:
      static RootFrame * ancestor();

      /** \brief Construct a RootFrame which wraps the given Root TGFrame.
          \param parent The parent frame in which to embed the constructed frame.
          \param receiver The event receiver which will process events from the frame.
          \param frame Already constructed Root object which the constructed frame will wrap.
          \param delete_parent Flag indicating frame owns (and should delete) parent.
      */
      RootFrame(IFrame * parent, IEventReceiver * receiver, TGFrame * frame, bool delete_parent = false);

      /** \brief Construct a top level RootFrame which wraps the given Root TGFrame.
          \param receiver The event receiver which will process events from the frame.
          \param frame Already constructed Root object which the constructed frame will wrap.
          \param delete_parent Flag indicating frame owns (and should delete) parent.
      */
      RootFrame(IEventReceiver * receiver, TGFrame * frame, bool delete_parent = false);

      /// \brief Destruct the frame.
      virtual ~RootFrame();

      /// \brief Display this frame and all it contains.
      virtual void display();

      /// \brief Hide this frame and all it contains.
      virtual void unDisplay();

      /** \brief Add the given (sub) frame to this container frame.
          \param frame The frame being added.
      */
      virtual void addFrame(IFrame * frame);

      /** \brief Remove the given (sub) frame to this container frame. If the frame is not currently in the container,
                 no harm done.
          \param frame The frame being removed.
      */
      virtual void removeFrame(IFrame * frame);

      /// \brief Get the horizontal center of the frame.
      virtual long getHCenter() const;

      /// \brief Set the horizontal center of the frame.
      virtual void setHCenter(long center);

      /// \brief Get the vertical center of the frame.
      virtual long getVCenter() const;

      /// \brief Set the vertical center of the frame.
      virtual void setVCenter(long center);

      /// \brief Get the X position of the left edge of the frame.
      virtual long getL() const;

      /** \brief Set the X position of the left edge of the frame.
          \param l The new position of the left edge.
      */
      virtual void setL(long l);

      /// \brief Get the X position of the right edge of the frame.
      virtual long getR() const;

      /** \brief Set the X position of the left edge of the frame.
          \param l The new position of the left edge.
      */
      virtual void setR(long r);

      /// \brief Get the Y position of the top edge of the frame.
      virtual long getT() const;

      /** \brief Set the Y position of the top edge of the frame.
          \param t The new position of the top edge.
      */
      virtual void setT(long t);

      /// \brief Get the Y position of the bottom edge of the frame.
      virtual long getB() const;

      /** \brief Set the Y position of the bottom edge of the frame.
          \param b The new position of the bottom edge.
      */
      virtual void setB(long b);

      virtual long getWidth() const;

      virtual void setWidth(long width);

      virtual long getHeight() const;

      virtual void setHeight(long height);

      virtual long getMinimumWidth() const;

      virtual void setMinimumWidth(long width);

      virtual long getMinimumHeight() const;

      virtual void setMinimumHeight(long height);

      /** \brief Handle mouse click event by forwarding it to receiver. Not part of the API.
      */
      virtual void clicked();

      /** \brief Handle closeWindow event by forwarding it to receiver. Not part of the API.
      */
      virtual void closeWindow();

      /** \brief Handle modified event by forwarding it to receiver. Not part of the API.
      */
      virtual void modified(const char * text);

      /** brief Set text owned by frame. Not part of the API.
          \param text The new text.
      */
      virtual void setText(const char * text);

      /// \brief Get underlying Root frame. Not part of the API.
      virtual TGFrame * getTGFrame();

    protected:
      std::list<RootFrame *> m_subframes;
      RootFrame * m_parent;
      TGFrame * m_frame;
      IEventReceiver * m_receiver;
      bool m_delete_parent;
      long m_minimum_width;
      long m_minimum_height;

    private:
      // Constructs a frame without any parents. This is a singleton.
      RootFrame();
  };

}

#endif
