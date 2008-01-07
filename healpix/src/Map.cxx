/** @file Map.cxx
@brief Wrapper for the JPL healpix class of a sky map, including Optimal Filters for the sphere

@author M. Roth 

$Header$
*/

#include "healpix/Map.h"
#include "healpix/AlmOp.h"
#include "healpix/HealPixel.h"
#include "base/healpix_map.h"
#include "base/healpix_map_fitsio.h"
#include "base/alm_map_tools.h"
#include "base/alm_powspec_tools.h"
#include "base/alm_filter_tools.h"
#include "base/powspec.h"
#include "base/fitshandle.h"

using namespace healpix;

template<typename T> Map<T>::Map(const std::string &file, int level):m_factor(2.35), m_hm(level,::RING) {
    fitshandle inp(file,2);
    read_Healpix_map_from_fits(inp,m_hm);
}

template Map<float>::Map(const std::string &file, int level);
template Map<double>::Map(const std::string &file, int level);


template<typename T> Map<T>::Map(int level):m_factor(2.35),m_hm(level,::RING) {
}

template Map<float>::Map(int level);
template Map<double>::Map(int level);

template<typename T> Map<T>::Map(const astro::SkyFunction& f,int level):m_factor(2.35),m_hm(level,::RING) {
    for(int i(0);i< m_hm.Npix();++i) {
        m_hm[i]=f(healpix::HealPixel(m_hm.ring2nest(i),level));
    }
}

template Map<float>::Map(const astro::SkyFunction& f, int level);
template Map<double>::Map(const astro::SkyFunction& f, int level);

template<typename T> Healpix_Map<T>* Map<T>::map() {
    return &m_hm;
}

template Healpix_Map<float>* Map<float>::map();
template Healpix_Map<double>* Map<double>::map();

template<typename T> std::vector<T> Map<T>::powspec(int lmax) {
    Alm<xcomplex<T> > alms(lmax,lmax);
    PowSpec ps(1,lmax);
    map2alm_iter(m_hm,alms,0);
    extract_powspec(alms,ps);
    std::vector<T> vals;
    for(int i=1;i<=lmax;++i) {
        vals.push_back(ps.tt(i));
    }
    return vals;
}

template std::vector<float> Map<float>::powspec(int lmax);
template std::vector<double> Map<double>::powspec(int lmax);

template<typename T> void Map<T>::scale(T factor) {
    
    for(int i(0);i<m_hm.Npix();++i) {
        m_hm[i]*=factor;
    }
}

template void Map<float>::scale(float factor);
template void Map<double>::scale(double factor);

template<typename T> void Map<T>::zeromap() {
    //zero negative elements
    for(int i(0);i<m_hm.Npix();++i) {
        if(m_hm[i]<0) m_hm[i]=0;
    }
}

template void Map<float>::zeromap();
template void Map<double>::zeromap();

template<typename T> Map<T> Map<T>::operator+(Map<T> &other) {
    Map<T> rmap(m_hm.Order()>=other.map()->Order()?m_hm.Order():other.map()->Order());
    if(other.map()->Order()==map()->Order()) {
        //maps have same resolution, do simple addition
        for(int i(0);i<m_hm.Npix();++i) {
            (*rmap.map())[i] = m_hm[i]+(*other.map())[i];
        }
    } else {
        for(int i(0);i<(m_hm.Order()>other.map()->Order()?m_hm.Npix():other.map()->Npix());++i) {
            //use higher resolution of the two maps to do weighted addition
            if(m_hm.Order()>other.map()->Order()) {
                int diff = (1 << 2*(m_hm.Order()-other.map()->Order()));
                (*rmap.map())[i] = diff*m_hm[i]+other(HealPixel(m_hm.ring2nest(i),m_hm.Order()));
            } else {
                int diff = (1 << 2*(other.map()->Order()-m_hm.Order()));
                (*rmap.map())[i] = m_hm[m_hm.nest2ring(HealPixel(HealPixel(other.map()->ring2nest(i),other.map()->Order()),m_hm.Order()).index())]+diff*(*other.map())[i];
            }
        }
    }
    return rmap;
}

template Map<float> Map<float>::operator+(Map<float> &other);
template Map<double> Map<double>::operator +(Map<double> &other);

template<typename T> void Map<T>::mfcn(const std::string &psf, int lmax) {
    AlmOp<xcomplex<T> > skylm(lmax,lmax);  //original map harmonics
    AlmOp<xcomplex<T> > psflm(lmax,lmax);  //point spread funtion harmonics
    Healpix_Map<T> psfmap(m_hm.Order(),::RING);
    fitshandle inp(psf,0,2);
    read_Healpix_map_from_fits(inp,psfmap); //point spread fits file    LHOOD.fits or SMHW.fits for likelihood or mexican hat wavelets
    map2alm_iter(m_hm,*skylm.Alms(),0);
    double energy = s_minenergy*pow(m_factor,log(1.0*m_hm.Nside())/log(2.0)-s_minlevel)*sqrt(m_factor); //energy of bin computed by geometric mean
    double angle = 5.59*pow(1/energy+1/50000,0.8); //angular scale determined by likelihood analysis
    double R = tan(angle/2)/tan(45.0); //dilation operators defined by McEwen
    map2alm_iterdil(psfmap,*psflm.Alms(),0,R);
    mf_constantnoise(*skylm.Alms(),*psflm.Alms());// filter described by McEwen
    alm2map(*skylm.Alms(),m_hm);
}

template void Map<float>::mfcn(const std::string &psf,int lmax);
template void Map<double>::mfcn(const std::string &psf,int lmax);

template<typename T> void Map<T>::mfcn(int lmax) {
    AlmOp<xcomplex<T> > skylm(lmax,lmax);  //original map harmonics
    AlmOp<xcomplex<T> > psflm(lmax,lmax);  //point spread funtion harmonics
    Healpix_Map<T> psfmap = lhood<T>(m_hm.Order()); //calculates likelihood shape from function
    map2alm_iter(m_hm,*skylm.Alms(),0);
    double energy = s_minenergy*pow(m_factor,log(1.0*m_hm.Nside())/log(2.0)-s_minlevel)*sqrt(m_factor); //energy of bin computed by geometric mean
    double angle = 5.59*pow(1/energy+1/50000,0.8);//angular scale determined by likelihood analysis
    double R = tan(angle/2)/tan(45.0); //dilation operators defined by McEwen
    map2alm_iterdil(psfmap,*psflm.Alms(),0,R);
    mf_constantnoise(*skylm.Alms(),*psflm.Alms());// filter described by McEwen
    alm2map(*skylm.Alms(),m_hm);
}

template void Map<float>::mfcn(int lmax);
template void Map<double>::mfcn(int lmax);


template<typename T> void Map<T>::mfvn(const std::string &noise, int lmax) {
    AlmOp<xcomplex<T> > skylm(lmax,lmax);  //original map harmonics
    AlmOp<xcomplex<T> > psflm(lmax,lmax);  //point spread funtion harmonics
    AlmOp<xcomplex<T> > noiselm(lmax,lmax); //noise harmonics
    Healpix_Map<T> noisemap(m_hm.Order(),::RING);
    fitshandle inp(noise,2,2);
    read_Healpix_map_from_fits(inp,noisemap);
    Healpix_Map<T> psfmap = lhood<T>(m_hm.Order()); //calculates likelihood shape from function
    map2alm_iter(m_hm,*skylm.Alms(),0);
    map2alm_iter(noisemap,*noiselm.Alms(),0);
    double energy = s_minenergy*pow(m_factor,log(1.0*m_hm.Nside())/log(2.0)-s_minlevel)*sqrt(m_factor); //energy of bin computed by geometric mean
    double angle = 5.59*pow(1/energy+1/50000,0.8);//angular scale determined by likelihood analysis
    double R = tan(angle/2)/tan(45.0); //dilation operators defined by McEwen
    map2alm_iterdil(psfmap,*psflm.Alms(),0,R);
    mf_noise(*skylm.Alms(),*psflm.Alms(),*noiselm.Alms());// filter described by McEwen
    alm2map(*skylm.Alms(),m_hm);
}

template void Map<float>::mfvn(const std::string &noise, int lmax);
template void Map<double>::mfvn(const std::string &noise, int lmax);


template<typename T> void Map<T>::writemap(std::string &out) {
    fitshandle output(out,fitshandle::CREATE,READWRITE);
    write_Healpix_map_to_fits(output,m_hm,TDOUBLE);
    output.close();
}

template void Map<float>::writemap(std::string &out);
template void Map<double>::writemap(std::string &out);

