/** \file RootTabFolder.h
    \brief Interface for RootTabFolder class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootTabFolder_h
#define st_graph_RootTabFolder_h

#include "st_graph/ITabFolder.h"

class TGTab;

namespace st_graph {

  class IEventReceiver;
  class RootFrame;

  /** \class RootTabFolder
      \brief Interface for tabbed folder style widget, which can contain any number of tabbed folder sheets.
  */
  class RootTabFolder : public ITabFolder {
    public:
      RootTabFolder(RootFrame * parent, IEventReceiver * receiver);

      /** \brief Add a new tabbed folder sheet, which is owned by the RootTabFolder object. The corresponding IFrame,
                 which may be used to add widgets to the folder sheet, is returned.
          \param label The label to place on the tab.
      */
      virtual IFrame * addTab(const std::string & label);

      /** \brief Get pointer to the top-level frame.
      */
      virtual IFrame * getFrame();

      /** \brief Cause selected tabbed folder to be on top.
          \param tab The tab folder sheet to select.
      */
      virtual void select(IFrame * tab);

    private:
      RootFrame * m_frame;
      TGTab * m_tg_tab;
      IEventReceiver * m_receiver;
  };

}

#endif
