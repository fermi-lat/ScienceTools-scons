/** \file LinearBinner.h
    \brief Declaration of a linearly uniform interval binner.
*/
#ifndef evtbin_LinearBinner_h
#define evtbin_LinearBinner_h

#include "evtbin/Binner.h"

namespace evtbin {
  /** \class LinearBinner
      \brief Declaration of a linearly uniform interval binner.
  */
  class LinearBinner : public Binner {
    public:
      LinearBinner(double interval_begin, double interval_end, long num_bins);

      /** \brief Return the bin number for the given value.
          \param value The value being binned.
      */
      virtual long computeIndex(double value) const;

      /** \brief Return the number of bins currently defined.
      */
      virtual long getNumBins() const;

    private:
      double m_interval_begin;
      double m_interval_end;
      long m_num_bins;
  };

}

#endif
