/** \file PlotHist1D.cxx
    \brief Implementation of base class plotter for 1D histograms.
    \author James Peachey, HEASARC/GSSC
*/
#include "st_graph/PlotHist1D.h"

namespace st_graph {

  PlotHist1D::PlotHist1D(const std::string & title, unsigned int width, unsigned int height, const IntervalCont_t & intervals):
    m_intervals(intervals), m_title(title), m_width(width), m_height(height) {}

  PlotHist1D::~PlotHist1D() {}

}
