/** @file PhotonMap.cxx
@brief implementation of PhotonMap

$Header$
*/

#include "map_tools/PhotonMap.h"

#include <cmath>
#include <utility>

using namespace map_tools;
using astro::HealPixel;
using astro::SkyDir;
using astro::Photon;

PhotonMap::PhotonMap(double emin, double eratio, int nlevels, int minlevel)
: m_emin(emin)
, m_eratio(eratio)
, m_levels(nlevels)
, m_minlevel(minlevel)
, m_photons(0)
, m_pixels(0)
{}


PhotonMap::~PhotonMap()
{
    
}

void PhotonMap::addPhoton(const astro::Photon& gamma)
{
    if( gamma.energy() < m_emin) return;
    HealPixel p = pixel(gamma);
    iterator it = this->find(p);
    if( it==this->end()){
        // create a new pixel with the entry
        this->insert( value_type(p,1) );
        ++m_pixels;
    }else{
        // just increment the count
        ++(it->second); 
    }
    ++m_photons;
}

HealPixel PhotonMap::pixel(const astro::Photon& gamma)
{
    int i = floor(log(gamma.energy()/m_emin)/log(m_eratio));
    i= i>m_levels? m_levels : i;
    return HealPixel(gamma.dir(), i+m_minlevel);
}

// Return density for a given direction, in photons/area of base pixel.
double PhotonMap::density (const astro::SkyDir & sd) const
{
    double result(0), weight(1.0);

    for (int ebin = 0; ebin < m_levels; ++ebin, weight*=4.0) { 
        astro::HealPixel hpx(sd, ebin+m_minlevel);
        const_iterator mit = this->find(hpx);

        if (mit != this->end()) {
            result += weight* mit->second;;
        }
    }

    return result;
}