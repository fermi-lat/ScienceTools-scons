/** \file LinearBinner.cxx
    \brief Implementation of a linearly uniform interval binner.
*/
#include "evtbin/LinearBinner.h"

namespace evtbin {

  LinearBinner::LinearBinner(double interval_begin, double interval_end, long num_bins):
    m_interval_begin(interval_begin),
    m_interval_end(interval_end),
    m_num_bins(num_bins) {}

  long LinearBinner::computeIndex(double value) const {
    if (value < m_interval_begin || value >= m_interval_end) return -1;
    return long(m_num_bins * (value - m_interval_begin) / (m_interval_end - m_interval_begin));
  }

  long LinearBinner::getNumBins() const { return m_num_bins; }

}
