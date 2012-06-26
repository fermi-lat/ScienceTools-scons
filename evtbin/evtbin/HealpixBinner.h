/** \file HealpixBinner.h
    \brief Declaration of a Healpix binner.
*/



#ifndef evtbin_HealpixBinner_h
#define evtbin_HealpixBinner_h

#include <string>

#include "evtbin/Binner.h"
#include "astro/SkyDir.h"
#include "astro/SkyFunction.h"
#include <utility> 

namespace evtbin {
  /** \class HealpixBinner
      \brief Declaration of a Healpix binner.
  */
  class HealpixBinner : public Binner {
    public:
      /** \brief Construct a Healpix binner object.
          \param ordering The ordering scheme of the map.
          \param order The order of the map.
          \param lb Do we use equatorial coordinates.
          \param name Optional name of the quantity being binned.
      */

     HealpixBinner(std::string ordering, int order, bool lb, const std::string & name = std::string());

      /** \brief Return the bin number for the given value.
          \param value The value of coordinates to bin.
      */
      virtual long computeIndex(double coord1, double coord2 ) const;
      virtual long computeIndex(double value) const; ///////////////////////1D-Binner

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

      virtual ~HealpixBinner() throw();
 

    
  private:
      std::string m_ordering;
      int m_order;
      bool  m_lb;
      long m_num_bins;
  };

}

#endif
