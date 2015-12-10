/** @file SkyProj.h
@brief declaration of the class SkyProj

$Header$
=======
*/

#ifndef astro_HealpixProj_H
#define astro_HealpixProj_H

#include "astro/ProjBase.h"

// Include files
#include <utility> // for pair
#include <string>

#include "healpix_base.h"

// forward declaration
struct wcsprm;
namespace tip { class Header;}

namespace astro {

  class SkyDir;

    /** @class HealpixProj
    * @brief Map Projection wrapper class for healpix
    * @author E. Charles from T. Hierath's SkyPro
    *

    For celestial coordinates, HealpixProj does input and output in degrees with the latitude in the 
    range [-90,90] and longitudes in the range [0,360).  

    */

  class HealpixProj : public ProjBase  {
  public:
    ///Constructors
    
    /* @brief Constructor specified by HEALPIX parameters
       @param order is the order of the map (nside = 2^order)
       @param scheme is the healpix ordering scheme (RING or NEST)
       @param galactic if coords are to be interpreted as galactic
    */
    HealpixProj(int order, Healpix_Ordering_Scheme scheme, bool galactic=false);
    
    /* @brief Constructor specified by HEALPIX parameters
       @param nside is the HEALPIX nside parameter
       @param scheme is the healpix ordering scheme (RING or NEST)
       @param nside_dummy must be set to SET_NSIDE
       @param galactic if coords are to be interpreted as galactic       
    */
    HealpixProj(int nside, Healpix_Ordering_Scheme scheme, const nside_dummy, bool galactic=false);
    
    /* @brief Constructor that reads HEALPIX information from a FITS image extension
       @param fitsFile The FITS filename.
       @param extension The HDU extension name. If a null string,
       then the primary HDU is used.
    */
    HealpixProj(const std::string & fitsFile, const std::string & extension="");
 
    // Destructor
    virtual ~HealpixProj();

    /// copy constructor
    HealpixProj(const HealpixProj& other);

    // Clone operator
    virtual ProjBase* clone() const { return new HealpixProj(*this); }
    
    /** @brief tranform form world  to pixels with the given coordinates
       @param s1 ra or l, in degrees
       @param s2 dec or b, in degrees
       @return pair(x,y) in pixel coordinates
    */
    virtual std::pair<double,double> sph2pix(double s1, double s2)const;

    /** @brief Convert from one projection to another
        @param x1 projected equivalent to ra or l, in degrees
        @param x2 projected equivalent dec or b, in degrees
        @param projection used to deproject these coordinates
        @return pair(x,y) in new pixel coordinates
    */
    std::pair<double,double> pix2pix(double x1, double x2, const ProjBase& otherProjection) const {
      std::pair<double,double> s = otherProjection.pix2sph(x1,x2);
      return sph2pix(s.first,s.second);
    }

    /** @brief Does the inverse projection
        @param x1 projected equivalent to ra or l, in degrees
        @param x2 projected equivalent dec or b, in degrees
	@return pair(x,y) in spherical coordinates
    */
    virtual std::pair<double,double> pix2sph(double x1, double x2) const;
    
    /** @brief returns the range 
        @param xvar varies x if true, varies y if false
        @param x1 x or y coordinate to find y or x range respectively
    */
    virtual std::pair<double,double> range(double x1, bool xvar);
    
    /** @brief returns 0 if point (x1,x2) is in range */
    virtual int testpix2sph(double x1, double x2)const;
    
    /** @brief set appropriate keywords in the FITS header */
    virtual void setKeywords(tip::Header& header);

    /* access to the HEALPix representation */
    inline const Healpix_Base& healpix() const { return m_hpx; }

    /// Utility function to convert a SkyDir to a pointing object (used by healpix)
    pointing convertToPointing(const SkyDir& dir);

  private:    

    Healpix_Base m_hpx;  // The HEALPix pixelization

  };
  
} // namespace astro
#endif    // astro_HealpixProj_H


