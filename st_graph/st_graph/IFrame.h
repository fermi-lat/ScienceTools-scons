/** \file IFrame.h
    \brief Interface for IFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_IFrame_h
#define st_graph_IFrame_h
#include <list>
#include <string>

namespace st_graph {

  /** \class IFrame
      \brief Interface for base class frame for all graphical frames.
  */
  class IFrame {
    public:
      /// \brief Destruct the frame.
      virtual ~IFrame() {}

      /// \brief Display this frame and all it contains.
      virtual void display() = 0;

      /// \brief Hide this frame and all it contains.
      virtual void unDisplay() = 0;

      /** \brief Add the given (sub) frame to this container frame.
          \param frame The frame being added.
      */
      virtual void addFrame(IFrame * frame) = 0;

      /** \brief Remove the given (sub) frame to this container frame. If the frame is not currently in the container,
                 no harm done.
          \param frame The frame being removed.
      */
      virtual void removeFrame(IFrame * frame) = 0;

      virtual void getSubframes(std::list<IFrame *> & frame_cont) = 0;

      /** \brief Get a string describing the state of the widget. The possible values of the state string depend
                 on the exact type of widget being represented by the IFrame.

                 For buttons, possible states are "up" and "down".
                 For text entry frames, the state gives the text the user has currently entered.
      */
      virtual const std::string & getState() const = 0;

      /** \brief Change the internal state of the widget using the state description given by the argument.
                 Valid values of the state string depend on the exact type of widget being represented by the IFrame.

                 For buttons, possible states are "up" and "down".
                 For text entry frames, the state gives the text the user has currently entered.
          \param state The new state of widget being set.
      */
      virtual void setState(const std::string & state) = 0;

      /** \brief Set the tool tip text for the widget. Causes tool tips to appear/disappear automatically.
                 Currently, this is only implemented for buttons!
          \param text The text to display in the tool tip.
      */
      virtual void setToolTipText(const std::string & text) = 0;

      /// \brief Resize the frame to its natural dimensions.
      virtual void setNaturalSize() = 0;

      /// \brief Get the horizontal center of the frame.
      virtual long getHCenter() const = 0;

      /// \brief Set the horizontal center of the frame.
      virtual void setHCenter(long center) = 0;

      /// \brief Get the vertical center of the frame.
      virtual long getVCenter() const = 0;

      /// \brief Set the vertical center of the frame.
      virtual void setVCenter(long center) = 0;

      /// \brief Get the X position of the left edge of the frame.
      virtual long getL() const = 0;

      /** \brief Set the X position of the left edge of the frame.
          \param l The new position of the left edge.
      */
      virtual void setL(long l) = 0;

      /// \brief Get the X position of the right edge of the frame.
      virtual long getR() const = 0;

      /** \brief Set the X position of the right edge of the frame.
          \param l The new position of the right edge.
      */
      virtual void setR(long r) = 0;

      /// \brief Get the Y position of the top edge of the frame.
      virtual long getT() const = 0;

      /** \brief Set the Y position of the top edge of the frame.
          \param t The new position of the top edge.
      */
      virtual void setT(long t) = 0;

      /// \brief Get the Y position of the bottom edge of the frame.
      virtual long getB() const = 0;

      /** \brief Set the Y position of the bottom edge of the frame.
          \param b The new position of the bottom edge.
      */
      virtual void setB(long b) = 0;

      virtual long getWidth() const = 0;

      virtual void setWidth(long width) = 0;

      virtual long getHeight() const = 0;

      virtual void setHeight(long height) = 0;

      virtual long getMinimumWidth() const = 0;

      virtual void setMinimumWidth(long width) = 0;

      virtual long getMinimumHeight() const = 0;

      virtual void setMinimumHeight(long height) = 0;

  };

}

#endif
