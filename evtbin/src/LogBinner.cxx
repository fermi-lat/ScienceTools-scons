/** \file LogBinner.cxx
    \brief Implementation of a logarithmically uniform interval binner.
*/

#include <cmath>

#include "evtbin/LogBinner.h"

namespace evtbin {

  LogBinner::LogBinner(double interval_begin, double interval_end, long num_bins, const std::string & name):
    Binner(name),
    m_interval_begin(interval_begin),
    m_interval_end(interval_end),
    m_num_bins(num_bins) {}

  long LogBinner::computeIndex(double value) const {
    if (value < m_interval_begin || value >= m_interval_end) return -1;
    return long(m_num_bins * log(double(value) / m_interval_begin) / log(double(m_interval_end) / m_interval_begin));
  }

  long LogBinner::getNumBins() const { return m_num_bins; }

  Binner::Interval LogBinner::getInterval(long index) const {
    // Check bounds, and handle endpoints explicitly to avoid any round-off:
    if (index < 0 || index >= m_num_bins)
      return Binner::Interval(0., 0.);
    else if (index == 0)
      return Binner::Interval(m_interval_begin, 
        m_interval_begin * exp((1 + index) * log(m_interval_end/m_interval_begin) / m_num_bins));
    else if (index == m_num_bins - 1)
      return Binner::Interval(m_interval_begin * exp(index * log(m_interval_end/m_interval_begin) / m_num_bins),
        m_interval_end);

    return Binner::Interval(
      m_interval_begin * exp(index * log(m_interval_end/m_interval_begin) / m_num_bins),
      m_interval_begin * exp((1 + index) * log(m_interval_end/m_interval_begin) / m_num_bins)
    );
  }

  Binner * LogBinner::clone() const { return new LogBinner(*this); }

}
