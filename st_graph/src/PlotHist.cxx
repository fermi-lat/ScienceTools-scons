/** \file PlotHist.cxx
    \brief Implementation of base class plotter for all histograms.
    \author James Peachey, HEASARC/GSSC
*/
#include "st_graph/PlotHist.h"

namespace st_graph {

  PlotHist::PlotHist(const std::string & title, unsigned int width, unsigned int height): m_title(title), m_width(width),
    m_height(height) {}

  PlotHist::~PlotHist() {}

}
