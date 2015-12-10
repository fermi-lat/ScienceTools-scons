/** \file HealpixBinner.h
    \brief Declaration of a Healpix binner with some extended functionality
*/



#ifndef evtbin_HealpixBinner_h
#define evtbin_HealpixBinner_h

#include <string>
#include <vector>
#include <map>

#include "evtbin/Binner.h"
#include "astro/SkyDir.h"
#include "healpix_base.h"
#include "healpix/HealpixRegion.h"
#include <utility> 

namespace evtbin {
  /** \class HealpixBinner
      \brief Declaration of a Healpix binner.
  */
  class HealpixBinner : public Binner {
    public:
      /** \brief Construct a Healpix binner object.
          \param order The order of the map.
	  \param scheme The healpix ordering scheme
          \param lb Do we use equatorial coordinates.
	  \param region Reference direction for partial-sky mapping
          \param name Optional name of the quantity being binned.
      */
      HealpixBinner(int order, Healpix_Ordering_Scheme scheme, bool lb, 
		    const std::string & region,
		    const std::string & name = std::string());     
    
      /** \brief Construct a Healpix binner object.
          \param nside The HEALPix nside parameter (nside = 2^order)
	  \param scheme The healpix ordering scheme
          \param lb Do we use equatorial coordinates.
	  \param refDif Reference direction for partial-sky mapping
	  \param mapRadius Radius of sky to map (in degrees)
          \param name Optional name of the quantity being binned.
      */
      HealpixBinner(int nside, Healpix_Ordering_Scheme scheme, const nside_dummy dummy, bool lb, 
		    const std::string & region,
		    const std::string & name = std::string());     
      
      /** \brief Copy c'tor
      */
      HealpixBinner(const HealpixBinner& other);
    
      /** \brief Construct an existing file
       */
      HealpixBinner(const std::string& healpix_file,
		    const std::string& ext_name="SKYMAP");
      
  
    
      /** \brief Create copy of this object.
      */
      virtual Binner * clone() const;


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


      virtual ~HealpixBinner() throw();

      // set header keywords
      virtual void setKeywords(tip::Header & header) const;

      // Access
      inline const Healpix_Base& healpix() const { return m_hpx; }
      inline bool lb() const { return m_lb; }
      inline const healpix::HealpixRegion* region() const { return m_region; } 
      inline bool allSky() const { return m_region == 0; }
      inline const std::vector<int>& pixelIndices() const { return m_pixelIndices; }
      inline const std::map<int,int>& pixGlobalToLocalMap() const { return m_pixGlobalToLocalMap; }

  private:

      /** \brief, Computing the mapping if less that the whole sky is used
      */
      void setPixelMapping();

      Healpix_Base m_hpx;  // HEALPIX Projection object
      bool  m_lb; // use Galactic coordinates

      healpix::HealpixRegion* m_region; // Region for partial-sky mappings

      std::vector<int> m_pixelIndices;  // global pixel indices for partial-sky mappings
      std::map<int,int> m_pixGlobalToLocalMap;  // reverse index map for partial-sky mappings
  };

}

#endif
