#include "healpix/Map.h"
#include "healpix/AlmOp.h"
#include "base/healpix_map.h"
#include "base/healpix_map_fitsio.h"
#include "base/alm_map_tools.h"
#include "base/alm_powspec_tools.h"
#include "base/alm_filter_tools.h"
#include "base/powspec.h"
#include "base/fitshandle.h"

using namespace healpix;

template<typename T> Map<T>::Map<T>(const std::string &file, int level):m_factor(2.35) {
	const nside_dummy dummy;
	m_hm = Healpix_Map<T>(pow(2.0,1.0*level),::RING,dummy);
	fitshandle inp(file,2);
	read_Healpix_map_from_fits(inp,m_hm);
}

template Map<float>::Map<float>(const std::string &file, int level);
template Map<double>::Map<double>(const std::string &file, int level);


template<typename T> Map<T>::Map<T>(int level):m_factor(2.35) {
	const nside_dummy dummy;
	m_hm = Healpix_Map<T>(pow(2.0,1.0*level),::RING,dummy);
}

template Map<float>::Map<float>(int level);
template Map<double>::Map<double>(int level);


template<typename T> Healpix_Map<T>* Map<T>::map() {
	return &m_hm;
}

template Healpix_Map<float>* Map<float>::map();
template Healpix_Map<double>* Map<double>::map();


template<typename T> void Map<T>::mfcn(const std::string &psf, int lmax) {
	AlmOp<xcomplex<T> > skylm(lmax,lmax);  //original map harmonics
	AlmOp<xcomplex<T> > psflm(lmax,lmax);  //point spread funtion harmonics
	const nside_dummy dummy;
	Healpix_Map<T> psfmap(m_hm.Npix(),::RING,dummy);
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