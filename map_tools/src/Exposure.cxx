/** @file Exposure.cxx
    @brief Implementation of class Exposure

   $Header$
*/
#include "map_tools/Exposure.h"
#include "map_tools/HealpixArrayIO.h"
#include "tip/Table.h"
#include "astro/EarthCoordinate.h"

using namespace map_tools;


Exposure::Exposure(const std::string& inputfile, const std::string& tablename)
: SkyExposure(SkyBinner(2))
{
   setData( map_tools::HealpixArrayIO::instance().read(inputfile, tablename));
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

Exposure::Exposure(double pixelsize, double cosbinsize)
: SkyExposure(SkyBinner(side_from_degrees(pixelsize)))
{
}


void Exposure::fill(const astro::SkyDir& dirz, double deltat)
{
    SkyBinner::iterator is = data().begin();
    for( ; is != data().end(); ++is){ // loop over all pixels
        CosineBinner & pixeldata= *is; // get the contents of this pixel
        astro::SkyDir pdir = data().dir(is); // dir() is defined in HealpixArray.h
        double costh = pdir().dot(dirz());
        pixeldata.fill(costh, deltat); // fill() is defined in CosineBinner.h
    }
    addtotal(deltat);
}


void Exposure::write(const std::string& outputfile, const std::string& tablename)const
{
    map_tools::HealpixArrayIO::instance().write(data(), outputfile, tablename);
}

void Exposure::load(const tip::Table * scData, 
                    const GTIvector& gti, 
                    bool verbose) {
   

   tip::Table::ConstIterator it = scData->begin();
   const tip::ConstTableRecord & row = *it;
   long nrows = scData->getNumRecords();

   for (long irow = 0; it != scData->end(); ++it, ++irow) {
      if (verbose && (irow % (nrows/20)) == 0 ) std::cerr << ".";
      if( processEntry( row, gti)) break;
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
                if( stop < second){
                    fraction = (stop-first)/(stop-start); // overlap start of gti
                    break;
                }
                fraction = (second-first)/(stop-start); // gti subset of history
                break;
            }else {
                if( start > second) continue; // interval after gti 
                if( stop < second ) {
                    fraction = 1.0; break;  // fully contained
                }
                fraction = (second-start)/(stop-start);  // overlap end of gti
                break;
            }
 
        }
        done = fraction==0 && start > gti.back().second; 
    }
    if( fraction>0. ) {
        double ra, dec;
        row["ra_scz"].get(ra);
        row["dec_scz"].get(dec);
        fill(astro::SkyDir(ra, dec), deltat* fraction);
    }
    return done; 

}

