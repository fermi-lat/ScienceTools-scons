/** \file ITabFolder.h
    \brief Interface for ITabFolder class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_ITabFolder_h
#define st_graph_ITabFolder_h

#include <map>
#include <string>

namespace st_graph {

  class IFrame;

  /** \class ITabFolder
      \brief Interface for tabbed folder style widget, which can contain any number of tabbed folder sheets.
  */
  class ITabFolder {
    public:
      /** \brief Add a new tabbed folder sheet, which is owned by the ITabFolder object. The corresponding IFrame,
                 which may be used to add widgets to the folder sheet, is returned.
          \param label The label to place on the tab.
      */
      virtual IFrame * addTab(const std::string & label) = 0;

      /** \brief Get a new tabbed folder sheet, which is owned by the ITabFolder object. The corresponding IFrame,
                 which may be used to add widgets to the folder sheet, is returned.
          \param label The label to place on the tab.
      */
      virtual IFrame * getTab(const std::string & label) = 0;

      /** \brief Get pointer to the top-level frame.
      */
      virtual IFrame * getFrame() = 0;

      /** \brief Cause selected tabbed folder to be on top.
          \param tab The tab folder sheet to select.
      */
      virtual void select(IFrame * tab) = 0;

      /** \brief Get a container with all frames owned by the tab folder.
          \param tab_cont The output frame container.
      */
      virtual void getTabCont(std::map<std::string, IFrame *> & tab_cont) = 0;
  };

}

#endif
