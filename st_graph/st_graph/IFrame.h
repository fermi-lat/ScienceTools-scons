/** \file IFrame.h
    \brief Interface for IFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_IFrame_h
#define st_graph_IFrame_h

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

      /// \brief Get the X position of the left edge of the frame.
      virtual long getL() const = 0;

      /** \brief Set the X position of the left edge of the frame.
          \param l The new position of the left edge.
      */
      virtual void setL(long l) = 0;

      /// \brief Get the X position of the right edge of the frame.
      virtual long getR() const = 0;

      /** \brief Set the X position of the left edge of the frame.
          \param l The new position of the left edge.
      */
      virtual void setR(long r) = 0;
  };

}

#endif
