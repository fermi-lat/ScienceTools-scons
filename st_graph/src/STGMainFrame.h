/** \file STGMainFrame.h
    \brief Modified TGMainFrame for internal use only by Root graphics classes.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_STGMainFrame_h
#define st_graph_STGMainFrame_h

#include "TGFrame.h"

namespace st_graph {

  /** \class STGMainFrame
      \brief Modified TGMainFrame for internal use only by Root graphics classes.
  */
  class STGMainFrame : public TGMainFrame {
    public:
      /** \brief Construct a main frame.
          \param width The width of the frame, in pixels
          \param height The height of the frame, in pixels
      */
      STGMainFrame(unsigned int width, unsigned int height);

      virtual ~STGMainFrame();

      /** \brief This is the method called when the user closes a window.
          Overridden from the Root base class to be a no-op, because otherwise the underlying
          windows owned by the window manager are destroyed, resulting in a zombie TGMainFrame.
      */
      virtual void CloseWindow();
  };

}

#endif
