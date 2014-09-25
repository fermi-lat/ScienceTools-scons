/** \file MPLTabFolder.h
    \brief Interface for MPLTabFolder class.
    \author Tom Stephens, HEASARC/GSSC
*/
#ifndef st_graph_MPLTabFolder_h
#define st_graph_MPLTabFolder_h

#include <map>
#include "st_graph/ITabFolder.h"

//class TGTab;

namespace st_graph {

  class IEventReceiver;
  class IFrame;
  class MPLFrame;

  /** \class MPLTabFolder
      \brief Interface for tabbed folder style widget, which can contain any number of tabbed folder sheets.
  */
  class MPLTabFolder : public ITabFolder {
    public:
      MPLTabFolder(MPLFrame * parent, IEventReceiver * receiver);

      virtual ~MPLTabFolder();

      /** \brief Add a new tabbed folder sheet, which is owned by the MPLTabFolder object. The corresponding IFrame,
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

      /** \brief Get the name of the currently selected tabbed folder.
      */
      virtual std::string getSelected() const;

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
      MPLFrame * m_frame;
//      TGTab * m_tg_tab;
      IEventReceiver * m_receiver;
  };

}

#endif
