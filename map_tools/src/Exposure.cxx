/** @file Exposure.cxx
    @brief Implementation of class Exposure

   $Header$
*/
#include "map_tools/Exposure.h"
#include "astro/SkyDir.h"
#include "tuple/ITable.h"

#include "image/Fits_IO.h"
#include "image/Image.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <numeric>
#include <algorithm>
using namespace map_tools;

double  Exposure::Index::skybinsize=1;
int     Exposure::Index::ra_factor=360;
int     Exposure::Index::dec_factor=180;
int     Exposure::Index::cosfactor=40;
double  Exposure::Index::cosmin = 0;
double  Exposure::Index::costhetabinsize;

//------------------------------------------------------------------------------
Exposure::Exposure(double skybin, double costhetabin) : m_total(0)
{
    // set binsizes in the key
    Index::skybinsize = int(skybin);
    Index::ra_factor  = int(360./skybin);
    Index::dec_factor = int(180./skybin);
    Index::cosfactor  = int((1.-Index::cosmin)/costhetabin);

    //total size to reserve
    unsigned int size= Index::ra_factor * Index::dec_factor * Index::cosfactor;
    m_exposureMap.resize(size);
    std::cout << "Creating a exposure hypercube, size " << size 
        << "="<< Index::ra_factor << " x "<< Index::dec_factor << " x "<< Index::cosfactor << std::endl;
    std::fill(m_exposureMap.begin(), m_exposureMap.end(), 0);
}
//------------------------------------------------------------------------------
Exposure::Exposure(const ExposureCube& cube, double total)
: m_exposureMap(cube), m_total(total)
{
    unsigned int size= Index::ra_factor * Index::dec_factor * Index::cosfactor;

    std::cout << "Loaded exposure map from a hypercube, size is " << m_exposureMap.size();
    if( size != m_exposureMap.size() ) {
        throw std::invalid_argument("wrong size");
    }
#if 0
    double tot = std::accumulate(m_exposureMap.begin(), m_exposureMap.end(), 0.0);
    std::cout << ", average exposure: " << tot/m_exposureMap.size() << std::endl;
#endif
}
//------------------------------------------------------------------------------
Exposure::Exposure(const std::string& fits_file)
{
    FloatImg & cube = *dynamic_cast<FloatImg *>(Fits_IO::read(fits_file, "hypercube"));
    m_exposureMap = cube.data();

// Obtain Index static variables from FITS header keywords. Assume a
// standard ordering for ra, dec, cos(theta).
    cube.getValue("NAXIS1", Index::ra_factor);
    cube.getValue("NAXIS2", Index::dec_factor);
    cube.getValue("NAXIS3", Index::cosfactor);
    double rastep, decstep;
    cube.getValue("CDELT1", rastep);
    cube.getValue("CDELT2", decstep);
    if (fabs(rastep) == fabs(decstep)) {
       Index::skybinsize = fabs(decstep);
    } else {
       throw std::range_error(std::string("Exposure::Exposure(fitsfile): ")
                              + "step sizes in RA and Dec do not match.");
    }
// @todo Check for cos(theta) weighting.
    cube.getValue("CDELT3", Index::costhetabinsize);
    cube.getValue("CRVAL3", Index::cosmin);
    cube.getValue("TOTAL", m_total);

    unsigned int size= Index::ra_factor * Index::dec_factor * Index::cosfactor;

    std::cout << "Loaded exposure map from a FITS file " 
        << fits_file << ", size is " << m_exposureMap.size() << std::endl;

    if( size != m_exposureMap.size() ) {
        throw std::invalid_argument("wrong size");
    }
}


//------------------------------------------------------------------------------
double Exposure::operator()(double ra, double dec, const Aeff& fun)const
{
    ///integrate the exposure at (ra,dec):

    double currentExposure = 0.;
    int index = Index(ra,dec); 
    for( int i=0; i< Index::cosfactor; ++i){
        double cosTheta = Index::costheta_value(i), 
            aeff_val = fun(cosTheta),
            map_val =  m_exposureMap[index];
        currentExposure += map_val * aeff_val;
        index+= Index::ra_factor*Index::dec_factor;
    }
    return currentExposure;
}
//------------------------------------------------------------------------------
double Exposure::operator()(const astro::SkyDir& dir, const Aeff& fun)const
{ 
    return operator()(dir.ra(), dir.dec(), fun);
}

//------------------------------------------------------------------------------
void Exposure::add(const astro::SkyDir& pos, double deltat){ 

    if(deltat<=0) return;
    for(double lprime=-180.+0.5*Index::skybinsize; lprime <180. ; lprime += Index::skybinsize){
        for(double bprime=-90.+0.5*Index::skybinsize; bprime <90. ; bprime += Index::skybinsize){
            astro::SkyDir prime(lprime, bprime);
            double cosdiff = const_cast<astro::SkyDir&>(pos)()*prime();
            if( cosdiff> Index::cosmin)
                m_exposureMap[Index(lprime, bprime, cosdiff)]+=deltat;
        }
    }
    m_total += deltat;
}
