/** \file IPlot.h
    \brief Implementation of IPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#include "st_graph/IPlot.h"

namespace st_graph {

  Marker::Marker(): m_text(), m_x(0.), m_y(0.), m_color(BLUE) {}

  Marker::Marker(double x, double y, const std::string & text, int color): m_text(text), m_x(x), m_y(y), m_color(color) {}

  IPlot::~IPlot() {}

}
