/** \file OrderedBinner.h
    \brief Declaration of a binner with ordered but otherwise arbitrary bins.
*/
#ifndef evtbin_OrderedBinner_h
#define evtbin_OrderedBinner_h

#include <string>
#include <vector>

#include "evtbin/Binner.h"

namespace evtbin {
  /** \class OrderedBinner
      \brief Declaration of a binner with ordered but otherwise arbitrary bins.
  */
  class OrderedBinner : public Binner {
    public:
      typedef std::vector<Binner::Interval> IntervalCont_t;

      /** \brief Construct an ordered binner object.
          \param intervals Container of bin definitions.
          \param name Optional name of the quantity being binned.
      */
      OrderedBinner(const IntervalCont_t & intervals, const std::string & name = std::string());

      virtual ~OrderedBinner() throw();

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
      IntervalCont_t m_intervals;
  };

}

#endif
