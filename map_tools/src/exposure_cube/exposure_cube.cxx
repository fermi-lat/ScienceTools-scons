/** @file exposure_cube.cxx
@brief build the exposure_cube application

@author Toby Burnett
$Header$
*/
#include "tuple/ITable.h"

#include "map_tools/MapParameters.h"
#include "map_tools/Exposure.h"
#include "map_tools/ExposureHyperCube.h"
#include "astro/SkyDir.h"

#include <iostream>
using namespace map_tools;
namespace ecube{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    class ExposureMapParameters : public MapParameters 
    {
    public:
        ExposureMapParameters(int argc, char* argv[])
            : MapParameters(argc, argv)
        {
            getValue<double>("costhetabinsize"); // 40 bins in sqrt(1-cos(thetat))
            getValue<double>("skybinsize", 1.0); // 1 degree bins in ra and dec
            getValue<double>("tstart");
            getValue<double>("tstop");
        }
    };
}
//--------------------------------------------------------------------------
#if 0 // development example, @todo: use if requested 
class LoadExposureFromEgret
{
public:
    LoadExposureFromEgret( Exposure& exp ){}
    LoadExposureFromEgret( const ecube::ExposureMapParameters& pars,   Exposure& exp )
    {
        double 
            start = pars["tstart"], 
            stop = pars["tstop"];
        tuple::ITable::Factory& factory = *tuple::ITable::Factory::instance();
        tuple::ITable &tup = *factory(pars.eventFile(),"d2e");

        const double & ra = tup.selectColumn("ra_scz");
        const double & dec = tup.selectColumn("dec_scz");
        const double & elapsed_time = tup.selectColumn("elapsed_time");
        const double & livetime = tup.selectColumn("livetime");

        for( tuple::Iterator it=tup.begin(); it!=tup.end(); ++it){
            if( start < elapsed_time ) continue;
            if( stop!=0 && stop > elapsed_time ) break;
            exp.add( astro::SkyDir(ra, dec), livetime); 
        }
    }
};
#endif
//--------------------------------------------------------------------------
class LoadExposureFromGlast
{
public:
    LoadExposureFromGlast( const ecube::ExposureMapParameters& pars,   Exposure& exp )
    {
        double 
            tstart = pars["tstart"], 
            tstop = pars["tstop"];
        tuple::ITable::Factory& factory = *tuple::ITable::Factory::instance();
        tuple::ITable &tuple = *factory(pars.inputFile(),"Ext1");

        const double & ra = tuple.selectColumn("ra_scz");
        const double & dec = tuple.selectColumn("dec_scz");
        const double & start = tuple.selectColumn("start");
        const double & stop = tuple.selectColumn("stop");
        const double & livetime = tuple.selectColumn("livetime");


        for( tuple::Iterator it=tuple.begin(); it!=tuple.end(); ++it){
            if( start < tstart ) continue;
            if( stop > tstop ) break;
            double deltat = livetime > 0 ? livetime : stop-start;
            exp.add(astro::SkyDir(ra, dec), deltat); 
        }
    }
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int main(int argc, char * argv[]) {
    using namespace ecube;
    try{

        // read in, or prompt for, all necessary parameters
        ExposureMapParameters pars(argc, argv);

        // create the exposure, and fill it from the history file
        Exposure ex( pars["pixelsize"], pars["costhetabinsize"]);

        LoadExposureFromGlast(  pars, ex); 

        // create the fits output file from the Exposure file
        ExposureHyperCube cube(ex, pars.outputFile());

    }catch( const std::exception& e){
        std::cerr << "Error: ending with exception: " << typeid(e).name()<< " \""<< e.what() << "\""<<std::endl;
        return 1;
    }
    return 0;
}
