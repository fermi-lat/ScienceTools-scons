/** \file LinearBinner.cxx
    \brief Implementation of a linearly uniform interval binner.
*/

#include "evtbin/LinearBinner.h"

namespace evtbin {

  LinearBinner::LinearBinner(double interval_begin, double interval_end, long num_bins, const std::string & name):
    Binner(name),
    m_interval_begin(interval_begin),
    m_interval_end(interval_end),
    m_num_bins(num_bins) {}

  long LinearBinner::computeIndex(double value) const {
    if (value < m_interval_begin || value >= m_interval_end) return -1;
    return long(m_num_bins * (value - m_interval_begin) / (m_interval_end - m_interval_begin));
  }

  long LinearBinner::getNumBins() const { return m_num_bins; }

  Binner::Interval LinearBinner::getInterval(long index) const {
    // Check bounds, and handle endpoints explicitly to avoid any round-off:
    if (index < 0 || index >= m_num_bins)
      return Binner::Interval(0., 0.);
    else if (index == 0)
      return Binner::Interval(m_interval_begin, m_interval_begin + (1 + index) * (m_interval_end - m_interval_begin) / m_num_bins);
    else if (index == m_num_bins - 1)
      return Binner::Interval(m_interval_begin + index * (m_interval_end - m_interval_begin) / m_num_bins, m_interval_end);

    return Binner::Interval(
      m_interval_begin + index * (m_interval_end - m_interval_begin) / m_num_bins,
      m_interval_begin + (1 + index) * (m_interval_end - m_interval_begin) / m_num_bins
    );
  }

}
