/** \file STGLayoutManager.cxx
    \brief Modified TGLayoutManager for internal use by RootFrames which contain other frames.
    \author James Peachey, HEASARC/GSSC
*/
#include "TCollection.h"
#include "TGDimension.h"
#include "TGFrame.h"
#include "TGLayout.h"

#include "RootFrame.h"
#include "st_graph/IEventReceiver.h"

namespace st_graph {

  class STGLayoutManager : public TGLayoutManager {
    public:
      STGLayoutManager(IEventReceiver * receiver, RootFrame * frame, TGCompositeFrame * tg_frame):
        m_receiver(receiver), m_frame(frame), m_tg_frame(tg_frame), m_default_size(frame->getTGFrame()->GetSize()) {}

      virtual void Layout() {
        // Lay out the parent frame using the receiver.
        m_receiver->layout(m_frame);

        // Lay out each child using its native TGLayoutManager.
        TIter next(m_tg_frame->GetList());
        while(TGFrameElement * el = dynamic_cast<TGFrameElement *>(next())) {
          el->fFrame->MoveResize(el->fFrame->GetX(), el->fFrame->GetY(), el->fFrame->GetWidth(), el->fFrame->GetHeight());
        }

      }

      virtual TGDimension GetDefaultSize() const { return m_default_size; }

    private:
      IEventReceiver * m_receiver;
      RootFrame * m_frame;
      TGCompositeFrame * m_tg_frame;
      TGDimension m_default_size;
  }; 

}
