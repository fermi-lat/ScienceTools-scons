/** \file RootFrame.h
    \brief Interface for RootFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootFrame_h
#define st_graph_RootFrame_h

#include "st_graph/IFrame.h"

class TGFrame;

namespace st_graph {

  class RootEngine;

  /** \class RootFrame
      \brief Interface for base class frame for all graphical frames.
  */
  class RootFrame : public IFrame {
    public:
      static RootFrame * ancestor();

      // Construct a RootFrame which encapsulates the given Root TGFrame.
      RootFrame(RootEngine * engine, IFrame * parent, TGFrame * frame);

      /// \brief Destruct the frame.
      virtual ~RootFrame();

      /// \brief Display this frame and all it contains.
      virtual void display();

      /// \brief Hide this frame and all it contains.
      virtual void unDisplay();

      /// \brief Get underlying Root frame. Not part of the API.
      virtual TGFrame * getTGFrame();

    protected:
      RootEngine * m_engine;
      RootFrame * m_parent;
      TGFrame * m_frame;

    private:
      RootFrame();
  };

}

#endif
