/** @file Healpix.cxx
    @brief Healpix class implementation with code from WMAP

    @author B. Lesnick 
    $Header$
*/
/* Local Includes */
#include "astro/Healpix.h"
#include "healpix/pointing.h" ///< NASA library for ra,d

/* Standard Includes */
#include <numeric> // for accumulate

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <stdexcept>

using namespace astro;


//=========================================================================================
//  C++ interface funtions
//=========================================================================================
Healpix::Healpix(long nside, Healpix_Ordering_Scheme ord, SkyDir::CoordSystem coordsys)
    : m_nside(nside),
      m_ord(ord),
      m_coordsys(coordsys),
      m_heal(nside,ord,SET_NSIDE)
{}

void Healpix::pix2ang(long index, double &theta, double &phi)const
{
    pointing point = m_heal.pix2ang(index);
    theta = point.theta;
    phi = point.phi;
}

void Healpix::ang2pix(double theta, double phi, long &index)const
{
     index = m_heal.ang2pix(pointing(theta,phi));
}        


Healpix::Pixel::Pixel(const astro::SkyDir &dir, const Healpix& hp)
: m_healpix(&hp)
{
    // get theta, phi (radians) in appropriate coordinate system
    double theta, phi;
    if( hp.coordsys()==astro::SkyDir::EQUATORIAL){
        theta = M_PI/2- dir.dec()*M_PI/180.;
        phi = dir.ra()*M_PI/180;
    }else{  // galactic
        theta = M_PI/2- dir.b()*M_PI/180.;
        phi = dir.l()*M_PI/180;
    }
    // and look up the pixel number
    m_healpix->ang2pix(theta, phi,m_index);
}

Healpix::Pixel::operator astro::SkyDir ()const
{
    double theta,phi;
    m_healpix->pix2ang( m_index,theta,phi);
    // convert to ra, dec (or l,b)
    return astro::SkyDir( phi*180/M_PI, (M_PI/2-theta)*180/M_PI, m_healpix->coordsys() );
}

void Healpix::Pixel::neighbors(std::vector<Healpix::Pixel> & p) const
{
    fix_arr<int,8> result;
    
    p.clear();
    if (!(this->m_healpix->nested()))
        throw std::runtime_error("Nested ordering required to determine neighbors.");
    
    this->m_healpix->m_heal.neighbors(m_index,result);
    if(result[result.size()-1]  != -1) {
            p.push_back(Healpix::Pixel(result[result.size()-1], *(this->m_healpix)));
    }
    for (int i = 0; i < result.size()-1; ++i)
    {
        if(result[i]  != -1) {
            p.push_back(Healpix::Pixel(result[i], *(this->m_healpix)));
        }
    }   
}

double Healpix::integrate(const astro::SkyFunction& f)const
{
    return std::accumulate(begin(), end(), 0., Integrand(f));
}

void Healpix::findNeighbors(long index, std::vector<long> &p)
{
   
    if (!(nested()))
        throw std::invalid_argument("Healpix::findNeighbors -- Nested ordering required to determine neighbors.");

    p.clear();

    fix_arr<int,8> result;// local copy of the list
    m_heal.neighbors(index,result);
    long n[8];
    int nit=0;
    if(result[result.size()-1]!=-1) {
            n[0]=result[result.size()-1];
            nit++;
    }
    for (int i=0;i<result.size()-1;i++) {
        if(result[i]!=-1) {
            n[nit]=result[i];
            nit++;
        }
    }
    // now insert this list into the output vector
    std::copy(n,n+nit, std::back_insert_iterator<std::vector<long> >(p));
}



