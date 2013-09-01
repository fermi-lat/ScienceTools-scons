/** @file HealPixel.cxx
@brief Implement the HealPixel class

$Header$
*/

#include "healpix/HealPixel.h" 
#include "healpix/Healpix.h"
#include <stdexcept>

using namespace healpix;


astro::SkyDir::CoordSystem HealPixel::s_coordsys = astro::SkyDir::GALACTIC;

HealPixel::HealPixel(long index, int level)
: m_index(index)
, m_level(level)
{
}

HealPixel::HealPixel(const astro::SkyDir& dir, int level)
:m_level(level)
{
    healpix::Healpix hp( nside(), healpix::Healpix::NESTED, s_coordsys);

    // get theta, phi (radians) in appropriate coordinate system
    double theta, phi;
    if( s_coordsys==astro::SkyDir::EQUATORIAL){
        theta = M_PI/2- dir.dec()*M_PI/180.;
        phi = dir.ra()*M_PI/180;
    }else{  // galactic
        theta = M_PI/2- dir.b()*M_PI/180.;
        phi = dir.l()*M_PI/180;
    }
    // and set the pixel number
    hp.ang2pix( theta, phi, m_index);
    if( m_index<0 ){
        throw std::out_of_range("HealPixel::HealPixel: bad index");
    }
}


HealPixel::operator astro::SkyDir()const
{
    double theta, phi; 
    Healpix hp(nside(), healpix::Healpix::NESTED, s_coordsys);
    hp.pix2ang( index(), theta, phi);
    // convert to ra, dec (or l,b)
    return astro::SkyDir( phi*180/M_PI, (M_PI/2-theta)*180/M_PI, s_coordsys );
}

long HealPixel::lastChildIndex(int childLevel) const
{
    int leveldiff = childLevel - level();
    
    if (leveldiff < 0)
        throw std::runtime_error("Level for children cannot be less than my level.");
        
    return ((index() + 1) << (leveldiff * 2)) - 1;
}


bool HealPixel::operator<(const HealPixel& other)const
{
    int leveldiff = level()-other.level();
    if( leveldiff==0 ) return index() < other.index();
    if( leveldiff<0 ) {
        // my level is less: I follow if equal
        return (index() << -leveldiff*2) <= other.index();
    }else{
        // my level is greater: I'm less if these are equal
        return index() < (other.index() << leveldiff*2); 
    }
}

bool HealPixel::operator==(const HealPixel& other)const
{
    return level() == other.level() && index() == other.index();
}

bool HealPixel::operator!=(const HealPixel& other)const
{
    return level() != other.level() || index() != other.index();
}

bool HealPixel::operator<=(const HealPixel& other)const
{
    return *this < other || *this == other;
}

std::vector<HealPixel> HealPixel::neighbors() const
{
    std::vector<HealPixel> p;
    Healpix hp( nside(), healpix::Healpix::NESTED, s_coordsys);
    std::vector<long> neighbors;
    hp.findNeighbors(index(), neighbors);
    for (std::vector<long>::const_iterator i = neighbors.begin();
        i !=neighbors.end(); ++i)
    {
        p.push_back( HealPixel(*i, level()));
    }
    return p;
}


bool HealPixel::test()
{ 
    bool check1 = HealPixel(3,3) < HealPixel(1,2);

    bool check2 = HealPixel(4,3) < HealPixel(1,2);

    std::cout << "checks: " << check1 << ", " << check2 << std::endl;
    return check1 && !check2; 
}
