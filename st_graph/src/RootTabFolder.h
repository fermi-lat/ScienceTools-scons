/** \file RootTabFolder.h
    \brief Interface for RootTabFolder class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_RootTabFolder_h
#define st_graph_RootTabFolder_h

#include <map>
#include "st_graph/ITabFolder.h"

class TGTab;

namespace st_graph {

  class IEventReceiver;
  class IFrame;
  class RootFrame;

  /** \class RootTabFolder
      \brief Interface for tabbed folder style widget, which can contain any number of tabbed folder sheets.
  */
  class RootTabFolder : public ITabFolder {
    public:
      RootTabFolder(RootFrame * parent, IEventReceiver * receiver);

      virtual ~RootTabFolder();

      /** \brief Add a new tabbed folder sheet, which is owned by the RootTabFolder object. The corresponding IFrame,
                 which may be used to add widgets to the folder sheet, is returned.
          \param label The label to place on the tab.
      */
      virtual IFrame * addTab(const std::string & label);

      /** \brief Get a new tabbed folder sheet, which is owned by the ITabFolder object. The corresponding IFrame,
                 which may be used to add widgets to the folder sheet, is returned.
          \param label The label to place on the tab.
      */
      virtual IFrame * getTab(const std::string & label);

      /** \brief Get pointer to the top-level frame.
      */
      virtual IFrame * getFrame();

      /** \brief Cause selected tabbed folder to be on top.
          \param tab The tab folder sheet to select.
      */
      virtual void select(IFrame * tab);

      /** \brief Get a container with all frames owned by the tab folder.
          \param tab_cont The output frame container.
      */
      virtual void getTabCont(std::map<std::string, IFrame *> & tab_cont);

    private:
      std::map<std::string, IFrame *> m_tab;
      RootFrame * m_frame;
      TGTab * m_tg_tab;
      IEventReceiver * m_receiver;
  };

}

#endif
