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
#include "astro/SkyDir.h"
#include "st_app/IApp.h"

#include <iostream>
using namespace map_tools;


class ExposureCubeApp : public st_app::IApp {
public:
    ExposureCubeApp() : st_app::IApp("exposure_cube"){}

    //--------------------------------------------------------------------------
#if 0 // development example, @todo: use if requested 
    class LoadExposureFromEgret
    {
    public:
        LoadExposureFromEgret( Exposure& exp ){}
        LoadExposureFromEgret( const MapParameters& pars,   Exposure& exp )
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
        LoadExposureFromGlast( const MapParameters& pars,   Exposure& exp )
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

    void run()
    {
        // read in, or prompt for, all necessary parameters
        MapParameters pars(this->hoopsGetParGroup());

        // create the exposure, and fill it from the history file
        Exposure ex( pars["pixelsize"], pars["costhetabinsize"]);

        LoadExposureFromGlast(  pars, ex); 

        // create the fits output file from the Exposure file
        ExposureHyperCube cube(ex, pars.outputFile());


    }
}application;
