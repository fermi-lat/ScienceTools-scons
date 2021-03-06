/** @file Band.cxx
@brief implement class Band 

$Header$
*/

#include "skymaps/Band.h"
#include "skymaps/PsfSkyFunction.h"

#include "healpix/Healpix.h"
#include <stdexcept>

using namespace skymaps;
using healpix::Healpix;
using astro::SkyDir;

int Band::cache_pix_counts(0);
bool Band::m_enable_cache(false);
int Band::cache_pix(){return Band::cache_pix_counts;}

bool Band::enable_cache(bool b) {
    bool t = m_enable_cache;
    m_enable_cache = b;
    return t;
}

Band::Band(int nside)
            : m_nside(nside)
            , m_event_class(0)
            , m_emin(0)
            , m_emax(0)
            , m_sigma(0)
            , m_gamma(0)
	      , m_gamma2(-1)
	      , m_sigma2(-1)
	      , m_frac2(0)
            , m_healpix(new healpix::Healpix(m_nside,Healpix::RING, SkyDir::GALACTIC))
        {}


Band::Band(int nside, int event_class, double emin,double emax,
            double sigma, double gamma, double sigma2, double gamma2, double frac2)
            : m_nside(nside)
            , m_event_class(event_class)
            , m_emin(emin)
            , m_emax(emax)
            , m_sigma(sigma)
            , m_gamma(gamma)
	      , m_gamma2(gamma2)
	      , m_sigma2(sigma2)
	      , m_frac2(frac2)
            , m_healpix(new healpix::Healpix(m_nside,Healpix::RING, SkyDir::GALACTIC))
        {}



Band::Band(const Band& other)
: m_nside(1)
{
    add(other);
}

Band::~Band()
{
	delete m_healpix;
}

void Band::add(const astro::SkyDir& dir, int count)
{
    m_pixels[index(dir)]+=count;
}
void Band::add(int i, int count)
{
    m_pixels[i]+=count;
}
double Band::operator()(const astro::SkyDir& dir)const
{
    PixelMap::const_iterator it = m_pixels.find(index(dir));
    return it == m_pixels.end() ? 0 : it->second;
}

astro::SkyDir Band::dir( int index)const
{
    Healpix::Pixel pix(index,*m_healpix);

    return pix();
}

int Band::index(const astro::SkyDir& dir)const
{
    Healpix::Pixel pix(dir,*m_healpix);

    return pix.index();
}


int Band::query_disk(const astro::SkyDir&sdir, double radius, 
                     std::vector<std::pair<astro::SkyDir,int> > & vec, bool include_empty)const
{
    std::vector<int> v;
    m_healpix->query_disc( sdir, radius, v); 
    int total(0);
    cache_pix_counts = v.size();
    // Add selected pixels to return vector
    for (std::vector<int>::const_iterator it = v.begin(); it != v.end(); ++it) {

        PixelMap::const_iterator it2 = m_pixels.find(*it);
        if( it2 != m_pixels.end() )  {
            int count = it2->second;
            vec.push_back( std::make_pair(dir(it2->first), count));
            total += count;
 
        }else if(include_empty ){
            vec.push_back(std::make_pair(dir(*it), 0));
        }
    }
    return total;
}


int Band::query_disk(const astro::SkyDir&sdir, double radius, 
                     std::vector<std::pair<int,int> > & vec, bool include_empty)const
{
    std::vector<int> v;
    m_healpix->query_disc( sdir, radius, v);
    cache_pix_counts = v.size();
    int total(0);
    // Add selected pixels to return vector
    for (std::vector<int>::const_iterator it = v.begin(); it != v.end(); ++it) {

        PixelMap::const_iterator it2 = m_pixels.find(*it);
        if( it2 != m_pixels.end() )  {
            int count = it2->second;
            vec.push_back( std::make_pair(it2->first, count));
            total += count;
         // option to return empty pixels within range
        }else if(include_empty){
            vec.push_back(std::make_pair(*it, 0));

        }
    }
    return total;
}

int Band::total_pix(const astro::SkyDir&dir, double radius)const
{
	std::vector<int> v;
	m_healpix->query_disc( dir, radius, v);
	return v.size();
}

double Band::pixelArea()const
{
   return m_healpix->pixelArea();
}

int Band::photons()const
{
    int count(0);
    for( PixelMap::const_iterator it=m_pixels.begin(); it!=m_pixels.end(); ++it){
        count += it->second;
    }
    return count;
}

// source ids
void Band::add_source(int source_id){
  bool found=false;
  for( std::vector<std::pair<int,int> >::iterator it=m_source.begin(); it!=m_source.end(); ++it){
    if(it->first==source_id){
      it->second++;
      found=true;
      break;
    }
  }
  if(!found)
    m_source.push_back(std::pair<int,int>(source_id,1));
}

void Band::findNeighbors(int index, std::vector<int> &neighbors)const
{
    m_healpix->findNeighbors(index, neighbors);
}
 
void Band::add(const Band& other)
{
    if( m_nside==1){
        // default ctor: making a copy
        m_nside = other.nside();
        m_healpix = new healpix::Healpix(m_nside,Healpix::RING, SkyDir::GALACTIC);
        m_sigma = other.sigma();
        m_gamma = other.gamma();
        m_emin = other.emin();
        m_emax = other.emax();
        m_sigma2 = other.sigma2();
        m_gamma2 = other.gamma2();
        m_frac2  = other.frac2();
        m_source = other.source(); // source ids
		m_event_class = other.event_class();
    }

    for( Band::const_iterator it= other.begin(); it!=other.end(); ++it){
        add(it->first, it->second);
    }
}

double Band::density(const astro::SkyDir& sd, bool smooth, int mincount, int kernel, double smooth_radius)const
{
    
    double value = 0.0, weight = 0.0;
    double cweight, d;
    int count;
    int my_index(index(sd));
    if (m_enable_cache) {
        std::map<int,double>::const_iterator it = m_density_cache.find(my_index);
        if (it != m_density_cache.end()){
            return it->second;
        }
    }

    if (!smooth) { 
        astro::SkyDir my_dir(dir(my_index));
        PixelMap::const_iterator it = m_pixels.find(my_index);
        value = ( it == m_pixels.end() ? 0 : it->second )/pixelArea();
        if (m_enable_cache) m_density_cache[my_index] = value;
        return value;
    }
       
    skymaps::PsfSkyFunction psf(sd,gamma(),sigma());
    std::vector<std::pair<astro::SkyDir,int> > v;
    query_disk(sd,smooth_radius*sigma(),v,true);
    bool neighbors_all_zero(true);
    for (std::vector<std::pair<astro::SkyDir,int> >::const_iterator n = v.begin(); n!= v.end(); ++n)
    {

        if (kernel == 0) { cweight = psf(n->first); }
        else {
            d = sd.difference(n->first);
            cweight = 1 / d;
        }

        count = n->second;
        neighbors_all_zero = neighbors_all_zero && (count == 0);

        weight += cweight;
        value  += (count<mincount?0:count) * cweight;
    }
        
    value = (neighbors_all_zero?0:value) / weight / pixelArea();
    if (m_enable_cache) m_density_cache[my_index] = value;
    return value;
    //return value / weight / pixelArea();
}

