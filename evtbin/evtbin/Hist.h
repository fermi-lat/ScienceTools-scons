/** \file Hist.h
    \brief Base class for histogram abstractions.
*/
#ifndef evtbin_Hist_h
#define evtbin_Hist_h

#include <vector>

namespace evtbin {

  class Binner;

  /** \class Hist
      \brief Base class for histogram abstractions.
  */
  class Hist {
    public:
      typedef std::vector<const Binner *> BinnerCont_t;

      virtual ~Hist() throw();

      /** \brief Increment the bin appropriate for the given value.
                 This is generic for N-dimensional histograms.
          \param value Vector giving the value being binned. The vector must have at least as
                 many values as the dimensionality of the histogram.
      */
      virtual void fillBin(const std::vector<double> & value) = 0;

      /** \brief Return the collection of binners being used by this histogram.
      */
      const BinnerCont_t & getBinners() const;

    protected:
      BinnerCont_t m_binners;
  };

}

#endif
