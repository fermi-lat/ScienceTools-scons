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


void Exposure::write(const std::string& outputfile, const std::string& tablename)
{
    map_tools::HealpixArrayIO::instance().write(data(), outputfile, tablename);
}
