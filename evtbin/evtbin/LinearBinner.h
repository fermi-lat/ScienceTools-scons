/** \file LinearBinner.h
    \brief Declaration of a linearly uniform interval binner.
*/
#ifndef evtbin_LinearBinner_h
#define evtbin_LinearBinner_h

#include <string>

#include "evtbin/Binner.h"

namespace evtbin {
  /** \class LinearBinner
      \brief Declaration of a linearly uniform interval binner.
  */
  class LinearBinner : public Binner {
    public:
      /** \brief Construct a linear binner object.
          \param interval_begin Left boundary of the binning interval.
          \param interval_end Right boundary of the binning interval.
          \param num_bins The number of bins to create.
          \param name Optional name of the quantity being binned.
      */
      LinearBinner(double interval_begin, double interval_end, long num_bins, const std::string & name = std::string());

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

    private:
      double m_interval_begin;
      double m_interval_end;
      long m_num_bins;
  };

}

#endif
