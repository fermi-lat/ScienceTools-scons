/** @file Exposure.cxx
    @brief Implementation of class Exposure

   $Header$
*/
#include "map_tools/Exposure.h"
#include "map_tools/HealpixArrayIO.h"


using namespace map_tools;

Exposure::Exposure(const std::string& inputfile, const std::string& tablename)
: SkyExposure(SkyBinner(2))
{
   setData( map_tools::HealpixArrayIO::instance().read(inputfile, tablename));
}

inline int side_from_degrees(double pixelsize){ return pixelsize>=10? 1 : 64; } 
/// todo: return the closest power of 2 for the side parameter
/// 41252 square degrees for the sphere
/// nside=64 is about one degee: 49152 pixels

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
