/** @file HealpixArrayIO.cxx
@brief Define the CosineBinner classixel 

@author T. Burnett

$Header$
*/

#include "map_tools/HealpixArrayIO.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#define TIP_ONECOLUMNBUG // remove this when tip is fixed to allow a single column

#include <cmath>
#include <sstream>
#include <string>
#include <typeinfo>

using namespace map_tools;

// Get the instance of the singleton class.
HealpixArrayIO & HealpixArrayIO::instance()
{
    static bool first_time = true;
    
    // Perform any initialization only once
    if (first_time)
    {
        first_time = false;
    }

    // Create the singleton class.
    static HealpixArrayIO s_HealpixArrayIO;

    return s_HealpixArrayIO;
}

void HealpixArrayIO::write(const astro::HealpixArray<CosineBinner> & ha,
                             const std::string & outputFile,
                             const std::string & tablename, bool clobber)
{
    if (clobber)
    {
        int rc = std::remove(outputFile.c_str());
        if( rc == -1 && errno == EACCES ) 
            throw std::runtime_error(std::string(" Cannot remove file " + outputFile));
    }

    // now add a table to the file
    tip::IFileSvc::instance().appendTable(outputFile, tablename);
    tip::Table & table = *tip::IFileSvc::instance().editTable( outputFile, tablename);

    // this is a work-around for a bug in tip v2r1p1
    #ifdef TIP_ONECOLUMNBUG
        table.appendField("DUMMY", "1E");
    #endif
    
    if (typeid(ha[0]) == typeid(CosineBinner)) // Just exploring during debugging
        int rc = rc;  
    std::stringstream ss;
    ss << ha[0].s_nbins << "E";
    std::string nbrbins = ss.str();
    table.appendField("COSBINS", ss.str());
    table.setNumRecords(ha.size());

    // get iterators for the Table and the HealpixArray
    tip::Table::Iterator itor = table.begin();
    astro::HealpixArray<map_tools::CosineBinner>::const_iterator haitor = ha.begin();

    // now just copy
    for( ; haitor != ha.end(); ++haitor, ++itor)
    {
        #ifdef TIP_ONECOLUMNBUG
                (*itor)["DUMMY"].set(0.);
        #endif
        (*itor)["COSBINS"].set(*haitor);
    }
    
    // set the headers (TODO: do the comments, too)
    tip::Header& hdr = table.getHeader();
    #ifdef TIP_ONECOLUMNBUG
        hdr["NAXIS1"].set((sizeof(float) * ha[0].s_nbins) + sizeof(float));
    #else
        hdr["NAXIS1"].set(sizeof(float) * ha[0].s_nbins);
    #endif
    hdr["PIXTYPE"].set("HEALPIX"); 
    hdr["ORDERING"].set("NESTED"); 
    hdr["NSIDE"].set(ha.healpix().nside()); 
    hdr["FIRSTPIX"].set(0); 
    hdr["LASTPIX"].set(ha.size()-1); 
    hdr["THETABIN"].set(CosineBinner::thetaBinning());
    hdr["NBRBINS"].set(CosineBinner::s_nbins);
    hdr["COSMIN"].set(CosineBinner::s_cosmin);

    // need to do this to close the file
    delete &table; 
}

 astro::HealpixArray<CosineBinner> HealpixArrayIO::read(const std::string & inputFile,
                                       const std::string & tablename)
{
    /* If the caller passes a reference to a HealpixArray instead of having this routine
       return a HealpixArray, how does the caller know what to set for nside, s_nbins, etc.?
       Could provide a GetAddtibutes function for this purpose.  If a reference is
       passed, this routine should throw an exception if the attributes of the
       HealpixArray passed don't match those of the file being read.  */
       
    const tip::Table & table=*tip::IFileSvc::instance().readTable(inputFile, tablename);
    const tip::Header& hdr = table.getHeader();
    int nside=0;
    hdr["NSIDE"].get(nside);
    std::string ordering;
    hdr["ORDERING"].get(ordering);
    astro::Healpix::Ordering ord = (ordering == "NESTED")?
        astro::Healpix::NESTED: astro::Healpix::RING;
    std::string thetabinstring;
    hdr["THETABIN"].get(thetabinstring);
    bool thetabin = (thetabinstring == "COSTHETA")? false: true;
    int nbrbins;
    hdr["NBRBINS"].get(nbrbins);
    double cosmin;
    hdr["COSMIN"].get(cosmin);
   
    CosineBinner::setBinning(cosmin, nbrbins, thetabin);
    astro::HealpixArray<map_tools::CosineBinner> ha(astro::Healpix(nside, ord));
    
    tip::Table::ConstIterator itor = table.begin();
    astro::HealpixArray<CosineBinner>::iterator haitor = ha.begin();
    
    for( ; itor != table.end(); ++haitor, ++itor)
    {
        (*itor)["COSBINS"].get(*haitor);
    }
    delete &table; 
    return ha;
}


