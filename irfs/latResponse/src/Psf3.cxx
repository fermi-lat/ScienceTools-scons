/**
 * @file Psf3.cxx
 * @brief Revised PSF that is the sum of two King model functions.
 * See http://confluence.slac.stanford.edu/x/bADIAw.
 * In contrast to Psf2, this class interpolates the distributions 
 * rather than the parameters.
 *
 * $Header$
 */

#include "Psf3.h"

//namespace latResponse {

Psf3::Psf3(const std::string & fitsfile, bool isFront,
           const std::string & extname, size_t nrow) 
   : Psf2(fitsfile, isFront, extname, nrow) {
}

Psf3::Psf3(const Psf3 & rhs) : Psf2(rhs) {}

Psf3::~Psf3() {
}

double Psf3::value(const astro::SkyDir & appDir, 
                   double energy, 
                   const astro::SkyDir & srcDir, 
                   const astro::SkyDir & scZAxis,
                   const astro::SkyDir & scXAxis, 
                   double time) const {
   (void)(scXAxis);
   double sep(appDir.difference(srcDir)*180./M_PI);
   double theta(srcDir.difference(scZAxis)*180./M_PI);
   double phi(0);
   return value(sep, energy, theta, phi, time);
}

double Psf2::value(double separation, double energy, double theta,
                  double phi, double time) const {
   (void)(phi);
   (void)(time);
   double * my_pars(pars(energy, std::cos(theta*M_PI/180.)));
   return psf_function(separation*M_PI/180., my_pars);
}


} namespace // latResponse
