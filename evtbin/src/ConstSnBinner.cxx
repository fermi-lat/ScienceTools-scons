/** \file ConstSnBinner.cxx
    \brief Implementation of a linearly uniform interval binner.
*/

#include <cmath>

#include "evtbin/ConstSnBinner.h"

namespace evtbin {

  ConstSnBinner::ConstSnBinner(double interval_begin, double interval_end, double sn_ratio, double lc_emin, double lc_emax,
    const std::vector<double> & background_coeffs, const std::string & name):
    OrderedBinner(IntervalCont_t(1, Interval(interval_begin, interval_end)), name),
    m_background_coeff(background_coeffs),
    m_interval_begin(interval_begin),
    m_interval_end(interval_end),
    m_sn_ratio_squared(sn_ratio * sn_ratio),
    m_lc_emin(lc_emin),
    m_lc_emax(lc_emax),
    m_counts(0.),
    m_background(0.),
    m_previous_value(interval_begin) {}

  long ConstSnBinner::computeIndex(double value) const {
    // First make sure value is within the range.
    // Note that this interval is (] unlike all other binners which are [).
    if (m_interval_begin >= value || m_interval_end < value) return -1;

    // See if this energy is in the range being used for S/N.
    // if (m_lc_emin > energy || m_lc_emax <= energy) return m_intervals.size() - 1;

    // Calculate the change in the background at this time.
    double delta_back = 0.;
    double value_jplus1th = 1.;
    double previous_value_jplus1th = 1.;
    for (unsigned int exponent = 0; exponent < m_background_coeff.size(); ++exponent) {
      value_jplus1th *= value;
      previous_value_jplus1th *= m_previous_value;
      delta_back += m_background_coeff[exponent] * (value_jplus1th - previous_value_jplus1th) / (exponent + 1);
    }

    // Add the increase to the cumulative background.
    m_background += delta_back;

    // Increment the number of counts.
    ++m_counts;

    // The current value becomes the previous value for next time.
    m_previous_value = value;

    // Compute amount by which background was exceeded.
    double differential_counts = m_counts > m_background ? m_counts - m_background : 0.;

    // Test whether S/N threshold has been exceeded.
    if (m_sn_ratio_squared * m_counts > differential_counts * differential_counts) return m_intervals.size() - 1;

    // Threshold was exceeded, so start a new S/N bin with no counts, no bg, and new interval.
    m_counts = 0.;
    m_background = 0.;

    // Current interval terminates with this value.
    m_intervals.back() = Interval(m_intervals.back().begin(), value);

    // Next interval starts with this value and ends with the interval_end supplied to the constructor.
    m_intervals.push_back(Interval(value, m_interval_end));

    return m_intervals.size() - 2;
  }

  long ConstSnBinner::getNumBins() const { return m_intervals.size(); }

  Binner::Interval ConstSnBinner::getInterval(long index) const {
    // Check bounds, and handle endpoints explicitly to avoid any round-off:
    if (index < 0 || (unsigned long)(index) >= m_intervals.size())
      return Binner::Interval(0., 0.);

    return m_intervals[index];
  }

  Binner * ConstSnBinner::clone() const { return new ConstSnBinner(*this); }

}
