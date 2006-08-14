/** \file IPlot.h
    \brief Implementation of IPlot class.
    \author James Peachey, HEASARC/GSSC
*/
#include "st_graph/IPlot.h"

namespace st_graph {

  int Color::nextColor(int current_color) {
    // Go to the next number in sequence.
    ++current_color;

    // Skip yellow, as it doesn't show up well.
    if (eYellow == current_color) ++current_color;

    // Wrap when the last color is reached.
    if (eNumberOfColors == current_color) current_color = eBlack;

    return current_color;
  }

  Marker::Marker(): m_text(), m_x(0.), m_y(0.), m_color(Color::eBlack) {}

  Marker::Marker(double x, double y, const std::string & text, int color): m_text(text), m_x(x), m_y(y), m_color(color) {}

  IPlot::~IPlot() {}

}
