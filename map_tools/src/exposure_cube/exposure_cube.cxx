/** @file exposure_cube.cxx
@brief build the exposure_cube application

@author Toby Burnett
$Header$
*/

#include "map_tools/MapParameters.h"
#include "map_tools/Exposure.h"
#include "map_tools/ExposureHyperCube.h"
#include "astro/SkyDir.h"
#include "astro/SkyDir.h"
#include "astro/GPS.h"
#include "astro/EarthCoordinate.h"
#include "tip/Table.h"
#include "tip/IFileSvc.h"

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/AppParGroup.h"

#include <iostream>
using namespace map_tools;


class ExposureCubeApp : public st_app::StApp {
public:
     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   /** \brief create application object, performing initializations needed for running the application.
    */
    ExposureCubeApp()
        : st_app::StApp()
        , m_pars(st_app::StApp::getParGroup("exposure_cube")) {
    }
           
     ~ExposureCubeApp() throw() {}  // needed since StApp has empty throw.

    //--------------------------------------------------------------------------
    void LoadExposureFromGlast( const MapParameters& pars,   Exposure& exp )
    {
        using tip::Table;

        double 
            tstart = pars["tstart"], 
            tstop = pars["tstop"];

        bool isText = pars.inputFile().find(".txt") >0;
        if( isText) {
            std::clog << "Opening textformat pointing history file " 
                << pars.inputFile() << std::endl;
            // read from text file here
            GPS& gps = *GPS::instance();
            gps.setPointingHistoryFile(pars.inputFile());
            const std::map<double,GPS::POINTINFO>& history = gps.getHistory();
            GPS::history_iterator mit = history.begin(), next=mit;
            double begintime=mit->first;
            double endtime = (--(history.end()))->first;
            
            double deltat = (++next)->first-begintime; 

            int added=0, total=0;
            for( ; mit!=history.end(); ++mit)
            {
                const GPS::POINTINFO& pt = mit->second;
                double t = mit->first, fraction= (t-begintime)/(endtime-begintime);
                if( t < tstart) continue;
                if( t > tstop) break;
                total++;
                if( astro::EarthCoordinate(pt.lat, pt.lon).insideSAA()) continue;
                added++;
                exp.add( pt.dirZ, deltat);
            }

            std::clog << "Number of steps added: " << added << ", rejected in SAA: "<< (total-added) << std::endl;
            std::clog << "Total elapsed time: " << deltat*(total-added) << " seconds." << std::endl;
            return;

        }
        // connect to  input data
        const Table & table = *tip::IFileSvc::instance().readTable(pars.inputFile(), 
            m_pars.getValue<std::string>("sctable"));

        for (Table::ConstIterator it = table.begin(); it != table.end(); ++it) {

            Table::ConstRecord & record = *it;

            double start, stop;
            record["start"].get(start);
            record["stop"].get(stop);

            if( start < tstart ) continue;
            if( stop > tstop ) break;

            double ra =record["ra_scz"].get(), 
                dec=record["dec_scz"].get(),
                livetime=  record["livetime"].get();

            double deltat = livetime > 0 ? livetime : stop-start;
            exp.add(astro::SkyDir(ra, dec), deltat); 
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void run()
    {
        std::clog << "Creating an exposure object ...";
        // create the exposure, and fill it from the history file
        Exposure ex( m_pars["pixelsize"], m_pars["costhetabinsize"]);

        LoadExposureFromGlast(  m_pars, ex); 

        // create the fits output file from the Exposure file
        ExposureHyperCube cube(ex, m_pars.outputFile());

    }
    private:
        MapParameters m_pars;
};
// Factory which can create an instance of the class above.
st_app::StAppFactory<ExposureCubeApp> g_factory;

/** @page exposure_cube_guide exposure_cube users guide

Create a special "exposure cube".

-Input: a history file, either a FITS FT2 file, or an ascii table with the following format
  - time (sec)
  - (x,y,z) of orbital position (km)
  - (ra, dec) of Z-axis
  - (ra, dec) of x-axis
  - (ra, dec) of local zenith [seems redundant with position]
  - (lat, lon)
  - altitude (m)

-Output: a FITS hypercube, defined by the parameter file, 

@verbinclude exposure_cube.par

*/
