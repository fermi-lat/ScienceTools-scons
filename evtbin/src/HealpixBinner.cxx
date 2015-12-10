/** \file HealpixBinner.cxx
    \brief Implementation of a Healpix binner.
*/

#include <cmath>
#include <numeric> 
#include <string>
#include <stdexcept>

#include "evtbin/HealpixBinner.h"   

#include "astro/SkyDir.h"
#include "astro/HealpixProj.h"
#include "tip/IFileSvc.h"
#include "tip/Header.h"


// EAC, in case we want to use HEALPIX from inside the ScienceTools
#ifdef HAVE_HEALPIX_INTERNAL
#include "healpix/base/healpix_map.h"
#else
#include "healpix_map.h"
#endif
#include "healpix/HealpixRegion.h"


using namespace astro;

namespace evtbin {

  HealpixBinner::HealpixBinner(int order, Healpix_Ordering_Scheme scheme, bool lb, 
			       const std::string & region,
			       const std::string & name)
    :Binner(name),
     m_hpx(order,scheme),
     m_lb(lb),
     m_region( region.empty() ? 0 : new healpix::HealpixRegion(region) )
  {
    if(order<0||order>12) {
      throw std::runtime_error("Order needs to be positive and <=12"); 
    }
    setPixelMapping();
  }

  HealpixBinner::HealpixBinner(int nside, Healpix_Ordering_Scheme scheme, const nside_dummy dummy, bool lb, 
			       const std::string & region,
			       const std::string & name)
    :Binner(name),
     m_hpx(nside,scheme,dummy),
     m_lb(lb),
     m_region( region.empty() ? 0 : new healpix::HealpixRegion(region) )
  {
    if(nside<0||nside>4096) {
      throw std::runtime_error("nside needs to be positive and <=4096"); 
    }
    setPixelMapping();
  }

  HealpixBinner::HealpixBinner(const std::string& healpix_file,
			       const std::string& ext_name)
    :Binner("HEALPIX"),
     m_hpx(),
     m_lb(false),
     m_region(0)
  {

    // Build a HealpixProj from the keywords
    astro::HealpixProj theProj(healpix_file,ext_name);
    m_hpx.SetNside(theProj.healpix().Nside(),theProj.healpix().Scheme());
    m_lb = theProj.isGalactic();

    const tip::Extension * ext = 
      tip::IFileSvc::instance().readExtension(healpix_file,ext_name);
    const tip::Header & header = ext->getHeader();
    
    double refdir1(0.);
    double refdir2(0.);
    std::string regionString;
    try {
      header["HPXREGION"].get(regionString);
    } catch (...) {
      ;
    }
    m_region = regionString.empty() ? 0 : new healpix::HealpixRegion(regionString);
    setPixelMapping();
  }
     

  HealpixBinner::HealpixBinner(const HealpixBinner& other)
    :Binner(other.getName()),
     m_hpx(other.m_hpx),
     m_lb(other.m_lb),
     m_region(other.m_region ? new healpix::HealpixRegion(*other.m_region) : 0),
     m_pixelIndices(other.m_pixelIndices),
     m_pixGlobalToLocalMap(other.m_pixGlobalToLocalMap)
  {
  }
  
   
  long HealpixBinner::computeIndex(double coord1, double coord2) const 
  {
    // HEALPix expects theta and phi in degrees
    double phi = astro::degToRad(coord1);
    double theta = astro::degToRad( astro::latToTheta_Deg(coord2) );
    int index= m_hpx.ang2pix(pointing(theta,phi));  //Convert theta,phi to pix nbr
    if ( allSky() ) {
      return index;
    }
    // Not an all-sky map, convert to local pixel index
    std::map<int,int>::const_iterator itrFind = m_pixGlobalToLocalMap.find(index);    
    if ( itrFind == m_pixGlobalToLocalMap.end() ) {
      return -1;
    }
    return itrFind->second;
  } //end of compute index
  
  /////////////////////////////////////////////////////////////////////////1D-Binner
  long HealpixBinner::computeIndex(double value) const {
    if ( allSky() ) {
      if (value < 0 || value > m_hpx.Npix() ) return -1;
      return long(value);
    }
    // Not an all-sky map, convert to local pixel index
    std::map<int,int>::const_iterator itrFind = m_pixGlobalToLocalMap.find(int(value));    
    if ( itrFind == m_pixGlobalToLocalMap.end() ) {
      return -1;
    }
    return long(itrFind->second);
  }
  ////////////////////////////////////////////////////////////////////////////
  
  long HealpixBinner::getNumBins() const { 
    return allSky() ? m_hpx.Npix() : m_pixelIndices.size();
  }
  
  Binner::Interval HealpixBinner::getInterval(long index) const {
    return Binner::Interval(0., 0.);
  }
  
  Binner * HealpixBinner::clone() const { return new HealpixBinner(*this); } 
  
  HealpixBinner::~HealpixBinner() throw(){
    delete m_region;
  };

  void HealpixBinner::setPixelMapping() {
    m_pixelIndices.clear();
    m_pixGlobalToLocalMap.clear();
    if ( m_region == 0 ) {
      return;
    }

    m_region->getPixels(m_hpx,m_pixelIndices);
       
    // finally, fill the reverse map
    for ( int i(0); i < m_pixelIndices.size(); i++ ) {
      m_pixGlobalToLocalMap[ m_pixelIndices[i] ] = i;
    }
    
  }
  
  void HealpixBinner::setKeywords(tip::Header & header) const {
    // Build a HealpixProj and get it to set the keywords
    astro::HealpixProj theProj(m_hpx.Nside(),m_hpx.Scheme(),SET_NSIDE,m_lb);
    theProj.setKeywords(header);
    if ( allSky() ) {
      header["INDXSCHM"].set("IMPLICIT");
    } else {
      header["INDXSCHM"].set("EXPLICIT");
      m_region->setKeywords(header);
    }
  }
  

} //end of evtbin namespace

