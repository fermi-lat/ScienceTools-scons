/** \file IFrame.h
    \brief Interface for IFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_IFrame_h
#define st_graph_IFrame_h

#include <string>
#include <utility>
#include <vector>

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

  };

}

#endif
