// Credits:  
//
//	April 2003 Ben Dorman, L3-Communications EER Systems Inc.
//	e-mail: ccfits@legacy.gsfc.nasa.gov

#ifndef PRIMARYHDU_H
#define PRIMARYHDU_H 1


#include <vector>

#include "Primary.h"
#include "FitsService.h"

namespace table{

template <typename T>
class PrimaryHDU : public Primary
{
public:
    PrimaryHDU();
    PrimaryHDU(int bitpix, int naxis, const std::vector<long> &naxes, 
        bool extend, double zero, double scale);
    virtual PrimaryHDU<T> *clone() const;


    const std::vector<T>& image () const;
    void setImage(const std::vector<T>& inData);

    virtual void read(const FitsService *ioService, long firstRow=1, long nrows=-1,
        long firstElem=1);

    virtual void write(const FitsService *ioService, long firstRow=1);


protected:
    std::vector<T>  m_data;
};



template <typename T>
PrimaryHDU<T>::PrimaryHDU()
: Primary(),
m_data()
{
}


template <typename T>
PrimaryHDU<T>::PrimaryHDU( int bitpix, int naxis, 
                          const std::vector<long> &naxes, bool extend = true, double zero=0., double scale=1.0)
                          : Primary( bitpix, naxis, naxes, extend, zero, scale),
                          m_data()
{
}


template <typename T>
PrimaryHDU<T> *PrimaryHDU<T>::clone() const   
{ 
    return new PrimaryHDU<T>(*this); 
}


template <typename T>
void PrimaryHDU<T>::read(const FitsService *ioService, long firstRow, long nrows, long)
{
    ioService->makePrimaryCurrent();

    if (m_naxis > 0)
        ioService->readImage(firstRow, nrows, m_data);
}


template <typename T>
const std::vector<T>& PrimaryHDU<T>::image () const
{
    return m_data;
}


template <typename T>
void PrimaryHDU<T>::setImage(const std::vector<T>& inData)
{
    m_data.resize(inData.size());
    m_data = inData;
}


template <typename T>
void PrimaryHDU<T>::write(const FitsService *ioService, long firstRow)
{
    ioService->makePrimaryCurrent();
    writeAllKeys(ioService);

    ioService->writeImageHeader( m_bitpix, m_naxis, m_naxes, m_extend, m_zero, m_scale);
    ioService->writeImage(firstRow, m_data.size(), m_data);
}

}
#endif
