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

}
