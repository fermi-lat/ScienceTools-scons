/** \file ConstSnBinner.h
    \brief Declaration of a linearly uniform interval binner.
*/
#ifndef evtbin_ConstSnBinner_h
#define evtbin_ConstSnBinner_h

#include <string>
#include <vector>

#include "evtbin/OrderedBinner.h"

namespace evtbin {
  /** \class ConstSnBinner
      \brief Declaration of a linearly uniform interval binner.
  */
  class ConstSnBinner : public OrderedBinner {
    public:
      /** \brief Construct a linear binner object.
          \param interval_begin Left boundary of the binning interval.
          \param interval_end Right boundary of the binning interval.
          \param sn_ratio S/N ratio for each bin.
          \param lc_emin The minimum energy cutoff.
          \param lc_emax The maximium energy cutoff.
          \param name Optional name of the quantity being binned.
      */
      ConstSnBinner(double interval_begin, double interval_end, double sn_ratio, double lc_emin, double lc_emax,
        const std::vector<double> & background_coeffs = std::vector<double>(), const std::string & name = std::string());

      ~ConstSnBinner() throw() {}

      /** \brief Return the bin number for the given value.
          \param value The value being binned.
      */
      virtual long computeIndex(double value) const;

      /** \brief Return the number of bins currently defined.
      */
      virtual long getNumBins() const;

      /** \brief Return the interval spanned by the given bin.
          \param index The index indicating the bin number.
      */
      virtual Binner::Interval getInterval(long index) const;

      /** \brief Create copy of this object.
      */
      virtual Binner * clone() const;

    private:
      std::vector<double> m_background_coeff;
      double m_interval_begin;
      double m_interval_end;
      double m_sn_ratio_squared;
      double m_lc_emin;
      double m_lc_emax;
      mutable double m_counts;
      mutable double m_background;
      mutable double m_previous_value;
  };

}

#endif
