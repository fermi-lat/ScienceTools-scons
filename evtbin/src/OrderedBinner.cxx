/** \file OrderedBinner.cxx
    \brief Implementation of a binner with ordered but otherwise arbitrary bins.
*/

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "evtbin/OrderedBinner.h"

namespace evtbin {

  OrderedBinner::OrderedBinner(const IntervalCont_t & intervals, const std::string & name): Binner(name), m_intervals(intervals) {
    // Check over the bins to make sure they're in ascending order.
    for (IntervalCont_t::const_iterator itor = m_intervals.begin(); itor != m_intervals.end(); ++itor) {
      // Each interval must itself be ordered.
      if (itor->end() < itor->begin()) {
        std::ostringstream os;
        os << "OrderedBinner: interval number " << (itor - m_intervals.begin()) << " has invalid range [" << itor->begin() << ", " << itor->end() << ")" <<std::endl;
        throw std::runtime_error(os.str());
      }

      if (itor + 1 == m_intervals.end()) break;

      // Each interval must also be ordered with respect to the next interval.
      if (itor->end() > (itor + 1)->begin()) {
        std::ostringstream os;
        unsigned long interval_num = itor - m_intervals.begin();
        os << "OrderedBinner: end value of interval number " << interval_num << " (" << itor->end() << ") is > beginning value of interval " <<
          interval_num + 1 << " (" << (itor + 1)->begin() << ")" << std::endl;
        throw std::runtime_error(os.str());
      }
    }
  }

  OrderedBinner::~OrderedBinner() throw() {}

  long OrderedBinner::computeIndex(double value) const {
    // Find the first bin for which the value is < that bin's beginning value.
    IntervalCont_t::const_iterator bound = std::upper_bound(m_intervals.begin(), m_intervals.end(), value);

    // If this iterator points to the very first bin, the value does not belong in any bin.
    if (m_intervals.begin() == bound) return -1;

    // This iterator position is one past the only interval which might contain the value, so back up.
    --bound;

    // This bin by definition has a beginning value >= the value, so just check the end of the interval.
    if (bound->end() > value) return bound - m_intervals.begin();

    return -1;
  }

  long OrderedBinner::getNumBins() const { return m_intervals.size(); }

  Binner::Interval OrderedBinner::getInterval(long index) const {
    // Check bounds, and handle endpoints explicitly to avoid any round-off:
    if (index < 0 || (unsigned long)(index) >= m_intervals.size())
      return Binner::Interval(0., 0.);

    return m_intervals[index];
  }

  Binner * OrderedBinner::clone() const { return new OrderedBinner(*this); }

}
