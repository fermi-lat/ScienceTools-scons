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
, m_logeratio(log(eratio))
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
    int i( static_cast<int>(log(gamma.energy()/m_emin)/m_logeratio) );
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
int PhotonMap::extract(int level, const astro::SkyDir& dir, double radius, std::vector<std::pair<int, float> >& vec)const
{
    radius *= (M_PI / 180); // convert to radians
    int summary_level( m_minlevel - 1); // default level to test
    int nside( 1<< summary_level);
    int npix( 12 * nside * nside);
    int total(0);

    // Look at each summary pixel and decide whether to go deeper.
    for (int i = 0; i < npix; ++ i)  {
        astro::HealPixel hp(i, summary_level);
        double distance = hp().difference(dir);

        if (distance <= radius) {
            astro::HealPixel boundary(i + 1, summary_level);
            for( const_iterator it = lower_bound(hp);
                it != end() && it->first < boundary; ++it)
            {
                if( it->first.level() != level ) continue;
                double distance = (it->first)().difference(dir);
                int count = it->second;
                vec.push_back(std::make_pair(count, distance));
                total+=count;
            }
        }
    }
    return total;
}

