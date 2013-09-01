/** \file STGMainFrame.h
    \brief Modified TGMainFrame for internal use only by Root graphics classes.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_STGMainFrame_h
#define st_graph_STGMainFrame_h

#include "TGFrame.h"

namespace st_graph {

  class RootEngine;

  /** \class STGMainFrame
      \brief Modified TGMainFrame for internal use only by Root graphics classes.
  */
  class STGMainFrame : public TGMainFrame {
    public:
      /** \brief Construct a main frame.
          \param engine The top level application/engine object which controls this frame.
          \param width The width of the frame, in pixels
          \param height The height of the frame, in pixels
      */
      STGMainFrame(RootEngine * engine, unsigned int width, unsigned int height);

      /** \brief This is the method called when the user closes a window. This method causes
          *all* frames to be hidden before terminating the Root event loop.
          Overridden from the Root base class.
      */
      virtual void CloseWindow();

    private:
      RootEngine * m_engine;
  };

}

#endif
