/** \file RootPlotFrame.h
    \brief Interface for RootPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootPlotFrame_h
#define st_graph_RootPlotFrame_h

#include <set>

#include "RootFrame.h"

#include "st_graph/IFrame.h"

class TGMainFrame;
class TRootEmbeddedCanvas;

namespace st_graph {

  class RootEngine;
  class RootFrame;

  /** \class RootPlotFrame
      \brief Interface for base class frame for all graphical frames.
  */
  class RootPlotFrame : public RootFrame {
    public:
      typedef std::set<RootPlot *> PlotCont_t;

      RootPlotFrame(IFrame * parent, unsigned int width, unsigned int height);

      /// \brief Destruct the frame.
      virtual ~RootPlotFrame();

      /** \brief Add the given (sub) frame to this container frame. The IFrame must really contain a plot, not some other widget.
                 This will go away soon.
          \param frame The frame being added.
      */
      virtual void addFrame(IFrame * frame);

      /** \brief Remove the given (sub) frame to this container frame. If the frame is not currently in the container,
                 no harm done. The IFrame must really contain a plot, not some other widget.
                 This will go away soon.
          \param frame The frame being removed.
      */
      virtual void removeFrame(IFrame * frame);

    private:
      TRootEmbeddedCanvas * m_canvas;
  };

}

#endif
