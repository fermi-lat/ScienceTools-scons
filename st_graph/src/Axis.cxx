/** \file Axis.cxx
    \brief Implementation of Axis class.
    \author James Peachey, HEASARC/GSSC
*/
#include "st_graph/Axis.h"

namespace st_graph {

  const std::string & Axis::getTitle() const { return m_title; }

  void Axis::setTitle(const std::string & title) { m_title = title; }

}
