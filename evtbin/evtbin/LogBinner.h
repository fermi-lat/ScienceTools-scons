/** \file LogBinner.h
    \brief Declaration of a logarithmically uniform interval binner.
*/
#ifndef evtbin_LogBinner_h
#define evtbin_LogBinner_h

#include "evtbin/Binner.h"

namespace evtbin {
  /** \class LogBinner
      \brief Declaration of a logarithmically uniform interval binner.
  */
  class LogBinner : public Binner {
    public:
      LogBinner(double interval_begin, double interval_end, long num_bins);

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
