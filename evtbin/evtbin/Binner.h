/** \file Binner.h
    \brief Base class for all binners.
*/
#ifndef evtbin_Binner_h
#define evtbin_Binner_h

namespace evtbin {
  /** \class Binner
      \brief Base class for all binners.
  */
  class Binner {
    public:
      virtual ~Binner() throw() {}

      /** \brief Return the bin number for the given value.
          \param value The value being binned.
      */
      virtual long computeIndex(double value) const = 0;

      /** \brief Return the number of bins currently defined.
      */
      virtual long getNumBins() const = 0;
  };

}

#endif
