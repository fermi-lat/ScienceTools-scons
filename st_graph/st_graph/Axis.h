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
      enum { eLinear, eLog };

      Axis();

      /** \brief Get the current title of the axis.
      */
      const std::string & getTitle() const;

      /** \brief Set the title of the axis.
          \param title The new title.
      */
      void setTitle(const std::string & title);

      int getScaleMode() const;

      void setScaleMode(int scale_mode);

    private:
      std::string m_title;
      bool m_log_scale;
  };
}

#endif
