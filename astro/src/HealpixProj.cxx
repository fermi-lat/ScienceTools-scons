/** @file HealpixProj.cxx
@brief implementation of the class HealpixProj

$Header$
*/

// Include files

#include <cstring>
#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Header.h"
#include "astro/HealpixProj.h"
#include "astro/SkyDir.h"

#include "fitsio.h"

using namespace astro;
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <cassert>
#include <iostream>

HealpixProj::HealpixProj(int order, Healpix_Ordering_Scheme scheme, bool galactic)
  : ProjBase("FHP",galactic,HEALPIX),m_hpx(order,scheme){
}

HealpixProj::HealpixProj(int nside, Healpix_Ordering_Scheme scheme, const nside_dummy dummy, bool galactic)
  : ProjBase("FHP",galactic,HEALPIX),m_hpx(nside,scheme,dummy){
}

HealpixProj::HealpixProj(const std::string & fitsFile, const std::string & extension)
  : ProjBase(false,HEALPIX) {

   const tip::Extension * ext = 
      tip::IFileSvc::instance().readExtension(fitsFile, extension);
   const tip::Header & header = ext->getHeader();

   bool galactic;
   int nside;
   int order;
   std::string ordering;
   std::string pixtype;
   std::string coordsys;
   int firstpix;
   int lastpix;
   Healpix_Ordering_Scheme scheme(RING);

   header["PIXTYPE"].get(pixtype);
   if ( pixtype != "HEALPIX" ) {
     throw std::runtime_error("HealpixProj called for extension with PIXTYPE == " + pixtype + ": " + fitsFile);
   }
   header["ORDERING"].get(ordering);
   if ( ordering == "RING" ) {
     scheme = RING;
   } else if (ordering  == "NESTED" ) {
     scheme = NEST;
   } else { 
     throw std::runtime_error("HealpixProj called for extension with ORDERING == " + ordering + ": " + fitsFile);
   }
   header["ORDER"].get(order);
   header["NSIDE"].get(nside);
   if ( order > 0 ) {
     if ( nside != int(pow(2,order)) ) {
       throw std::runtime_error("HealpixProj NSIDE does not match order ORDER: "+ fitsFile);
     }
     m_hpx.Set(order,scheme);
   } else {
     // non-standard nside ( not equal to 2^n ), we could warn, but this is fine
     m_hpx.SetNside(nside,scheme);
   }
   header["COORDSYS"].get(coordsys);
   if ( coordsys == "GAL" ) {
     galactic = true;
   } else if ( coordsys == "CEL" || coordsys == "EQU") {
     galactic = false;
   } else { 
     throw std::runtime_error("HealpixProj with COORDSYS == " + coordsys + ": " + fitsFile);
   }
   delete ext;
   initBase("FHP",galactic,ProjBase::HEALPIX);
}

HealpixProj::~HealpixProj(){;}

HealpixProj::HealpixProj(const HealpixProj& other) 
  : ProjBase(other),m_hpx(other.m_hpx.Nside(),other.m_hpx.Scheme(),SET_NSIDE){
}


std::pair<double,double> HealpixProj::sph2pix(double s1, double s2) const {
  // Holder for the return values
  std::pair<double,double> retVal(-1.,0.);
  // value checking.  Let's leave the longitude alone, but require the fabs(latitude) be <= 90.
  if ( fabs(s2) > 90. ) return retVal;
  // convert to what HEALPix expects (angles in radians and theta instead of latitude)
  double phi = astro::degToRad(s1); 
  double theta = astro::degToRad(astro::latToTheta_Deg(s2));
  pointing ang(theta,phi);
  // ask HEALPix for the pixel number
  retVal.first = m_hpx.ang2pix( ang );  
  return retVal;
}

std::pair<double,double> HealpixProj::pix2sph(double x1, double x2) const {
  // Holder for the return values
  std::pair<double,double> retVal(-1,0.);
  // This is really a 1-D representation, if x2 is not 0 we have a problem
  if ( fabs(x2) > 0.5 ) return retVal;  
  // Round x1 to the nearest integer. 
  int pix = nearbyint(x1);
  // ask HEALPix for the "pointing"
  pointing ang = m_hpx.pix2ang(pix);
  // put values in the ranges 0<=theta<=pi and 0<=phi<2*pi
  ang.normalize();  
  // switch to lon,lat  
  retVal.first = astro::radToDeg(ang.phi);
  retVal.second = astro::radToDeg(thetaToLat_Rad(ang.theta));  
  return retVal;
}

std::pair<double,double> HealpixProj::range(double x1,bool xvar) {
  std::pair<double,double> retVal(0.,0.);
  if (!xvar) {
    // X is constant. This is really a 1-D representation so return (0.,0.)
    return retVal;
  }
  // Y is constant.  Check that y == 0.  Note that in this case xvar mean the y coordinate.  Go figure 
  if ( fabs(xvar) > 0.5 ) return retVal;
  // The range is just 0,Npix().  Set the max (which strangely lives in retVal.first )
  retVal.first = double(m_hpx.Npix());
  return retVal;
}

int HealpixProj::testpix2sph(double x1, double x2) const{
  // this is supposed to return a mask telling us which coordinates are messed up
  // i.e., 0 means all ok
  int mask(0); 
  if ( x1 < 0 || x1 > m_hpx.Npix() ) mask |= 0x1; // (1st coordinate element is messed up)
  if ( fabs(x2) > 0.5 ) mask |= 0x2; // (2md coordinate element is messed up)
  return mask;
}

namespace {
  tip::Header* header;  
  template <typename T>
  void setKey(std::string name, T value, std::string unit="", std::string comment=""){
    (*header)[name].set( value); 
    (*header)[name].setUnit(unit);
    (*header)[name].setComment(comment);
  }
}

void HealpixProj::setKeywords(tip::Header& hdr) {
  header = &hdr;
  setKey("TELESCOP", "GLAST");  
  setKey("INSTRUME", "LAT");
  if ( isGalactic() ) {
    setKey("COORDSYS", "GAL");
  } else {
    setKey("EQUINOX", 2000.0,"","Equinox of RA & DEC specifications");
    setKey("COORDSYS", "CEL");
  } 
  setKey("ORDERING", m_hpx.Scheme() == RING ? "RING" : "NESTED");
  setKey("PIXTYPE", "HEALPIX");
  setKey("ORDER", m_hpx.Order());
  setKey("NSIDE", m_hpx.Nside());
  setKey("FIRSTPIX", 0);
  setKey("LASTPIX", m_hpx.Npix()-1);

}


pointing HealpixProj::convertToPointing(const SkyDir& dir) {
  // convert to what HEALPix expects (angles in radians and theta instead of latitude)
  double s1 = isGalactic() ? dir.l() : dir.ra();
  double s2 = isGalactic() ? dir.b() : dir.dec();
  double phi = astro::degToRad(s1); 
  double theta = astro::degToRad(astro::latToTheta_Deg(s2));
  return pointing(theta,phi);
}
