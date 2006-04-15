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
namespace tip { class Table; class ConstTableRecord;}

#include <utility> // for std::pair


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

    virtual ~BasicExposure(){}

    virtual void fill(const astro::SkyDir& dirz, double deltat)=0;
    virtual void fill(const astro::SkyDir& dirz, const astro::SkyDir& dirzenith, double deltat, double zcut)=0;

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
    Exposure(double pixelsize=1., double cosbinsize=1./CosineBinner::s_nbins);

    //! add a time interval at the given position
    virtual void fill(const astro::SkyDir& dirz, double deltat);

    /** @brief this was added by Julie, to allow horizon cut, possible if FOV includes horizon
        @param dirz direction of z-axis of instrument
        @param dirzenith direction of local zenith
        @param deltat time interval
        @param zcut (default acos(-0.4) = 113.6 degrees)
    */
    virtual void fill(const astro::SkyDir& dirz, const astro::SkyDir& dirzenith, double deltat, double zcut=-0.4);

    //! create object from the data file (FITS for now)
    Exposure(const std::string& inputfile, const std::string& tablename="Exposure");

    //! write out to a file.
    void write(const std::string& outputfile, const std::string& tablename="Exposure")const;

    typedef std::vector<std::pair<double, double> > GTIvector;

    //! load a set of history intervals from a table, qualified by a set of "good-time" intervals 
    void load(const tip::Table * scData, 
        const GTIvector & gti= GTIvector(), 
                    bool verbose=true);

private:
    bool processEntry(const tip::ConstTableRecord & row, const GTIvector& gti);

    /** @brief set up the cache of vectors associated with cosine histograms

    */
    void create_cache();

    /** @class Simple3Vector 
    @brief replacement for Hep3Vector for speed of dot product

    */
    class Simple3Vector {
    public: 
        Simple3Vector(const CLHEP::Hep3Vector& v=CLHEP::Hep3Vector())
            : x(v.x()),y(v.y()),z(v.z()){};
        double dot(const Simple3Vector& u)const{return x*u.x+y*u.y+z*u.z;}
        double x,y,z;
    };
    std::vector< std::pair<CosineBinner* ,  Simple3Vector> > m_dir_cache;
    class Filler ; ///< class used to fill a CosineBinner object with a value
};


} // namespace map_tools
#endif
