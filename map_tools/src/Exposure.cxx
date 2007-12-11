/** @file Exposure.cxx
    @brief Implementation of class Exposure

   $Header$
*/
#include "map_tools/Exposure.h"
#include "healpix/HealpixArrayIO.h"
#include "tip/Table.h"
#include "astro/EarthCoordinate.h"

#include <memory>
#include <algorithm>

using namespace map_tools;
using healpix::HealpixArrayIO;
using healpix::CosineBinner;


Exposure::Exposure(const std::string& inputfile, const std::string& tablename)
: SkyExposure(SkyBinner(2))
{
   setData( HealpixArrayIO::instance().read(inputfile, tablename));
}

/// return the closest power of 2 for the side parameter
/// 41252 square degrees for the sphere
/// nside=64 is about one degee: 49152 pixels
inline int side_from_degrees(double pixelsize){ 
    int n = 1;
    while( 12*n*n < 41252/(pixelsize*pixelsize) && n < 512){
        n *=2;
    }
    return n; 
} 

Exposure::Exposure(double pixelsize, double cosbinsize, double zcut)
: SkyExposure(SkyBinner(side_from_degrees(pixelsize)))
, m_zcut(zcut), m_lost(0)
{
    unsigned int cosbins = static_cast<unsigned int>(1./cosbinsize);
    if( cosbins != CosineBinner::nbins() ) {
        SkyBinner::iterator is = data().begin();
        for( ; is != data().end(); ++is){ // loop over all pixels
            CosineBinner & pixeldata= *is; // get the contents of this pixel
            pixeldata.resize(cosbins);
            CLHEP::Hep3Vector pixdir = data().dir(is)();
        }
        CosineBinner::setBinning(0, cosbins);
    }
    create_cache();
}

void Exposure::create_cache()
{
    size_t datasize(data().size());
    m_dir_cache.reserve(datasize);

    SkyBinner::iterator is = data().begin();
    for( ; is != data().end(); ++is){ // loop over all pixels
        Simple3Vector pixdir(data().dir(is)());
        m_dir_cache.push_back(std::make_pair(&*is, pixdir));
    }
}
/** @class Filler
    @brief private helper class used in for_each to fill a CosineBinner object
*/
class Exposure::Filler {
public:
    /** @brief ctor
        @param deltat time to add
        @param dir direction to use to determine angle (presumably the spacecraft z-axis)
        @param zenith optional zenith direction for potential cut
        @param zcut optional cut: if -1, ignore
    */
    Filler( double deltat, const astro::SkyDir& dir, astro::SkyDir zenith=astro::SkyDir(), double zcut=-1)
        : m_dir(dir())
        , m_zenith(zenith())
        , m_deltat(deltat)
        , m_zcut(zcut)
        , m_total(0), m_lost(0)
    {}
    void operator()( const std::pair<CosineBinner*, Simple3Vector> & x)
    {
        // check if we are making a horizon cut:
        bool ok( m_zcut==-1);
        if( ! ok) {
            double z(x.second.dot(m_zenith));
            ok = z > m_zcut;
        }
        if( ok) {
            // if ok, add to the angle histogram
            x.first->fill(x.second.dot(m_dir), m_deltat);
            m_total += m_deltat;
        }else{
            m_lost += m_deltat;
        }
    }
    double total()const{return m_total;}
    double lost()const{return m_lost;}
private:
    Simple3Vector m_dir, m_zenith;
    double m_deltat, m_zcut, m_total, m_lost;
};

void Exposure::fill(const astro::SkyDir& dirz, double deltat)
{
    Filler sum = for_each(m_dir_cache.begin(), m_dir_cache.end(), Filler(deltat, dirz));
    addtotal(deltat);
}


void Exposure::fill(const astro::SkyDir& dirz, const astro::SkyDir& zenith, double deltat)
{
    Filler sum = for_each(m_dir_cache.begin(), m_dir_cache.end(), Filler(deltat, dirz, zenith, m_zcut));
    double total(sum.total());
    addtotal(total);
    m_lost += sum.lost();
}


void Exposure::write(const std::string& outputfile, const std::string& tablename)const
{
    healpix::HealpixArrayIO::instance().write(data(), outputfile, tablename);
}

void Exposure::load(const tip::Table * scData, 
                    const GTIvector& gti, 
                    bool verbose) {
   
   tip::Table::ConstIterator it = scData->begin();
   const tip::ConstTableRecord & row = *it;
   long nrows = scData->getNumRecords();

   for (long irow = 0; it != scData->end(); ++it, ++irow) {
      if (verbose && (irow % (nrows/20)) == 0 ) std::cerr << ".";
      if( processEntry( row, gti) )break;
   }
   if (verbose) std::cerr << "!" << std::endl;
}


bool Exposure::processEntry(const tip::ConstTableRecord & row, const GTIvector& gti)
{
#if 0 // enable when use SAA?
    double latGeo, lonGeo;
    row["lat_Geo"].get(latGeo);
    row["lon_Geo"].get(lonGeo);
    astro::EarthCoordinate earthCoord(latGeo, lonGeo);
    if( earthCoord.insideSAA() ) return false;
#endif

    double  start, stop, livetime; 
    row["livetime"].get(livetime);
    row["start"].get(start);
    row["stop"].get(stop);
    double deltat = livetime > 0 ? livetime : stop-start;


    double fraction(1); 
    bool  done(false);
    if( !gti.empty() ) {
        fraction = 0;

        GTIvector::const_iterator it  = gti.begin();
        for ( ; it != gti.end(); ++it) {
            double first = it->first,
                second=it->second;

            if( start < first ) {
                if( stop < first) continue; // history interval before gti
                if( stop <= second){
                    fraction = (stop-first)/(stop-start); // overlap start of gti
                    break;
                }
                fraction = (second-first)/(stop-start); // gti subset of history
                break;
            }else {
                if( start > second) continue; // interval after gti 
                if( stop <= second ) {
                    fraction = 1.0; break;  // fully contained
                }
                fraction = (second-start)/(stop-start);  // overlap end of gti
                break;
            }
 
        }
        done = fraction==0 && start > gti.back().second; 
    }
    if( fraction>0. ) {
        double ra, dec, razenith, deczenith;
        row["ra_scz"].get(ra);
        row["dec_scz"].get(dec);
	row["ra_zenith"].get(razenith);
	row["dec_zenith"].get(deczenith);
        fill(astro::SkyDir(ra, dec), astro::SkyDir(razenith,deczenith), deltat* fraction);
    }
    return done; 

}

