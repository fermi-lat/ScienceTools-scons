/** \file Axis.cxx
    \brief Implementation of Axis class.
    \author James Peachey, HEASARC/GSSC
*/
#include "st_graph/Axis.h"

namespace st_graph {

  Axis::Axis(): m_title(), m_log_scale(false) {}

  const std::string & Axis::getTitle() const { return m_title; }

  void Axis::setTitle(const std::string & title) { m_title = title; }

  int Axis::getScaleMode() const { return m_log_scale ? eLog : eLinear; }

  void Axis::setScaleMode(int scale_mode) { m_log_scale = eLog == scale_mode ? true : false; }

}
