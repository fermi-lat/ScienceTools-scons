/** @file Exposure.h
    @brief definition of the class Exposure

    @author T.Burnett
    $Header$
*/
#ifndef MAP_TOOLS_EXPOSURE_H
#define MAP_TOOLS_EXPOSURE_H


#include "astro/SkyDir.h"
#include "astro/HealpixArray.h"
#include "map_tools/CosineBinner.h"

/** @class BasicExposure
@brief template for differential exposure

@param S Pixelization class, must implement dir(), is a list of C objects
@param C angualar binner class, must implement operator()(const F&)
@param F a function of one parameter
*/


template< class S, class C>
class BasicExposure {
public:
    BasicExposure(S sky):m_sky(sky), m_total(0){}

    virtual void fill(const astro::SkyDir& dirz, double deltat)=0;

    template<class F>
        double operator()(const astro::SkyDir& dir, const F& fun)const
    {
        const C& binner = m_sky[dir];
        return binner(fun);
    }
    const S& data()const{return m_sky;}
    S& data(){return m_sky;}
    double total()const{return m_total;}
    void addtotal(double t){ m_total+=t;}

    void setData(const S& data){m_sky=data;}
private:
    S m_sky;
    double m_total;
};

// define Exposure as specific instantiation of the above
typedef astro::HealpixArray<map_tools::CosineBinner> SkyBinner;
typedef BasicExposure<SkyBinner, map_tools::CosineBinner> SkyExposure;

namespace map_tools {

/**
@class Exposure
@brief Manage a differential exposure database.

It is a pixelated using Healpix binning, and the CosineBinner class


*/

class Exposure : public SkyExposure {
public:
    //! create object with specified binning
    //! @param pixelsize (deg) Approximate pixel size, in degrees
    //! @param cosbinsize bin size in the cos(theta) binner
    Exposure(double pixelsize, double cosbinsize);

    //! add a time interval at the given position
    virtual void fill(const astro::SkyDir& dirz, double deltat);

    //! create object from the data file (FITS for now)
    Exposure(const std::string& inputfile, const std::string& tablename="Exposure");

    //! write out to a file.
    void write(const std::string& outputfile, const std::string& tablename="Exposure");

private:
};


} // namespace map_tools
#endif
