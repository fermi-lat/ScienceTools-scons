/** \file Axis.h
    \brief Declaration of Axis class.
    \author James Peachey, HEASARC/GSSC
*/
#ifndef st_graph_Axis_h
#define st_graph_Axis_h

#include <string>

namespace st_graph {

  /** \class Axis
      \brief Represents a graphical axis, with title, tick marks, scale, etc.
  */
  class Axis {
    public:
      /** \brief Get the current title of the axis.
      */
      const std::string & getTitle() const;

      /** \brief Set the title of the axis.
          \param title The new title.
      */
      void setTitle(const std::string & title);

    private:
      std::string m_title;
  };
}

#endif
