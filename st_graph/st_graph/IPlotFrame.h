/** \file IPlotFrame.h
    \brief Interface for IPlotFrame class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_IPlotFrame_h
#define st_graph_IPlotFrame_h

#include "st_graph/IFrame.h"

namespace st_graph {

  class IPlot;

  /** \class IPlotFrame
      \brief Interface for base class frame for all graphical frames.
             This is not currently used, and for the moment, deprecated.
  */
  class IPlotFrame {
    public:
      /// \brief Destruct the frame.
      virtual ~IPlotFrame() {}

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

  };

}

#endif
