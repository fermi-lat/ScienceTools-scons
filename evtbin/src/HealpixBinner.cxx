/** \file HealpixBinner.cxx
    \brief Implementation of a Healpix binner.
*/

#include <cmath>
#include <numeric> 
#include <string>
#include <stdexcept>

#include "evtbin/HealpixBinner.h"   

#include "astro/SkyDir.h"
#include "astro/SkyProj.h"
#include "astro/SkyFunction.h"

#include "healpix/base/healpix_map.h"

#include "healpix/Healpix.h"
#include "healpix/HealPixel.h"

// anonymous namespace for helper functions
// pi here is pulled out from CLHEP via includes
namespace {
    inline double radians(double x){return x*pi/180.;}
    inline double degrees(double x){return x*180./pi;}
}

using namespace astro;
using namespace healpix;

namespace evtbin {

  HealpixBinner::HealpixBinner(std::string ordering, int order, bool lb, const std::string & name):
    Binner(name), m_ordering(ordering), m_order(order),
    m_lb(lb) 
  {
    if(order<0||order>12) {
      throw std::runtime_error("Order needs to be positive and <=12"); } else {
    int long Nside=pow((long double)2,order);
    m_num_bins = 12*Nside*Nside; 
    }
  }
   
  long HealpixBinner::computeIndex(double coord1, double coord2) const
     {
     //"Convert" string type into Healpix_Ordering_Scheme type
     Healpix_Ordering_Scheme  OrderingScheme;              
     if( m_ordering == "NESTED"){OrderingScheme=NEST;}
     else if (m_ordering == "RING"){OrderingScheme=RING;}
     
     //Create a Healpix_Base object
     Healpix_Base Hbase(m_order, OrderingScheme); 
     
 
     //Definition of dir:
     astro::SkyDir dir=SkyDir(0.,0.,astro::SkyDir::GALACTIC); 
     if(m_lb) dir=SkyDir(coord1,coord2,astro::SkyDir::GALACTIC);   
     else     dir=SkyDir(coord1,coord2,astro::SkyDir::EQUATORIAL);
     // Convert to pix nbr:
     double theta(pi/2.), phi(pi/180.);   //Convert skycoord to theta,phi
     if(m_lb){                             //if Galactic coordinate chosen
       theta -= radians(dir.b());
       phi   *= dir.l();
     }
     else{                                 //if Equatorial coordinate chosen
       theta -= radians(dir.dec());
       phi   *= dir.ra();
     }
     int long index=Hbase.ang2pix(pointing(theta,phi));  //Convert theta,phi to pix nbr
     return index;
     } //end of compute index
  
  /////////////////////////////////////////////////////////////////////////1D-Binner
  long HealpixBinner::computeIndex(double value) const {
    if (value < 0 || value >= 10) return -1;
    return long((value ) / 1.);
  }
  ////////////////////////////////////////////////////////////////////////////
  

  long HealpixBinner::getNumBins() const { return m_num_bins; }
  
  Binner::Interval HealpixBinner::getInterval(long index) const {
    // Check bounds, and handle endpoints explicitly to avoid any round-off:
    if (index < 0 || index >= m_num_bins)
      return Binner::Interval(0., 0.);
    else if (index == 0)
      return Binner::Interval(0., 0.);
    else if (index == m_num_bins - 1)
      return Binner::Interval(0., 0.);
    
    return Binner::Interval(0., 0.);
  }
  
  Binner * HealpixBinner::clone() const { return new HealpixBinner(*this); } 
  
  HealpixBinner::~HealpixBinner() throw(){};
  
} //end of evtbin namespace

