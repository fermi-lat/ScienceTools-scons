/** @file AlmOp.cxx
@brief Wrapper for the JPL healpix class of spherical harmonics, with the addition of operators required for filtering

@author M. Roth 

$Header$
*/

#include "healpix/AlmOp.h"

// EAC, update include path
#include "xcomplex.h"
#include "alm.h"

using namespace healpix;

template<typename T> healpix::AlmOp<T>::AlmOp() :m_alm(0,0) {
}

template AlmOp<xcomplex<float> >::AlmOp();
template AlmOp<xcomplex<double> >::AlmOp();


template<typename T> healpix::AlmOp<T>::AlmOp(int lmax, int mmax):m_alm(lmax,mmax) {
}
template AlmOp<xcomplex<float> >::AlmOp(int lmax, int mmax);
template AlmOp<xcomplex<double> >::AlmOp(int lmax, int mmax);


template<typename T> Alm<T>* AlmOp<T>::Alms() {
    return &m_alm;
}
template Alm<xcomplex<float> >* AlmOp<xcomplex<float> >::Alms();
template Alm<xcomplex<double> >* AlmOp<xcomplex<double> >::Alms();


template<typename T> AlmOp<T> AlmOp<T>::operator*(AlmOp<T> x) {
    planck_assert(m_alm.conformable(*x.Alms()),"Can't multiply different sized Alms!");
    AlmOp<T> product(m_alm.Lmax(),m_alm.Mmax());
    for(int l=0;l<=m_alm.Lmax();l++) {
        for(int m=0;m<=m_alm.Mmax();m++) {
            T *it = product.Alms()->mstart(m);
            it[l].real()=x.Alms()->mstart(m)[l].real()*m_alm.mstart(m)[l].real();
            it[l].imag()=x.Alms()->mstart(m)[l].imag()*m_alm.mstart(m)[l].imag();
        }
    }
    return product;
}

template AlmOp<xcomplex<float> > AlmOp<xcomplex<float> >::operator*(AlmOp<xcomplex<float> >);
template AlmOp<xcomplex<double> > AlmOp<xcomplex<double> >::operator*(AlmOp<xcomplex<double> >);


template<typename T> AlmOp<T> AlmOp<T>::operator+(AlmOp<T> x) {
    planck_assert(m_alm.conformable(*x.Alms()),"Can't add different sized Alms!");
    AlmOp<T> sum(m_alm.Lmax(),m_alm.Mmax());
    for(int l=0;l<=m_alm.Lmax();l++) {
        for(int m=0;m<=m_alm.Mmax();m++) {
            T *it = sum.Alms()->mstart(m);
            it[l].real()=x.Alms()->mstart(m)[l].real()+m_alm.mstart(m)[l].real();
            it[l].imag()=x.Alms()->mstart(m)[l].imag()+m_alm.mstart(m)[l].imag();
        }
    }
    return sum;
}

template AlmOp<xcomplex<float> > AlmOp<xcomplex<float> >::operator+(AlmOp<xcomplex<float> >);
template AlmOp<xcomplex<double> > AlmOp<xcomplex<double> >::operator+(AlmOp<xcomplex<double> >);


template<typename T> AlmOp<T> AlmOp<T>::conjugate() {
    AlmOp<T> cj(m_alm.Lmax(),m_alm.Mmax());	
    for(int l=0;l<=m_alm.Lmax();l++) {
        for(int m=0;m<=m_alm.Mmax();m++) {
            T *it = cj.Alms()->mstart(m);
            it[l].real()=m_alm.mstart(m)[l].real();
            it[l].imag()=-(m_alm.mstart(m)[l].imag());
        }
    }
    return cj;
}

template AlmOp<xcomplex<float> > AlmOp<xcomplex<float> >::conjugate();
template AlmOp<xcomplex<double> > AlmOp<xcomplex<double> >::conjugate();
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
