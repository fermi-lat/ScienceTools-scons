/** @file exposure_cube.cxx
@brief build the exposure_cube application

@author Toby Burnett
$Header$
*/

#include "map_tools/Parameters.h"
#include "map_tools/Exposure.h"
#include "map_tools/HealpixArrayIO.h"

#include "astro/SkyDir.h"
#include "astro/GPS.h"
#include "astro/EarthCoordinate.h"

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/AppParGroup.h"

#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

#include <iostream>
using namespace map_tools;


class ExposureCubeApp : public st_app::StApp {
public:
     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   /** \brief create application object, performing initializations needed for running the application.
    */
    ExposureCubeApp()
        : st_app::StApp()
        , m_pars(st_app::StApp::getParGroup("exposure_cube"))
        , m_f("ExposureCubeApp", "", 2)

    {}
    ~ExposureCubeApp() throw() {} // required by StApp with gcc

    //--------------------------------------------------------------------------
    void LoadExposureFromGlast( const Parameters& pars,   Exposure& exp )
    {

        double 
            tstart = pars["tstart"], 
            tstop = pars["tstop"];

        bool isText = pars.inputFile().find(".txt") != std::string::npos;
        bool avoid_saa = pars["avoid_saa"]!=0;

        m_f.info() << "Opening " << (isText? "text":"FITS") << " format pointing history file " 
                << pars.inputFile() << std::endl;
        // read from text or FITS file here
        GPS& gps = *GPS::instance();
        gps.setPointingHistoryFile(pars.inputFile());
        const std::map<double,GPS::POINTINFO>& history = gps.getHistory();
        GPS::history_iterator mit = history.begin(), next=mit;
        double begintime=mit->first;
        double endtime = (--(history.end()))->first;

        double deltat = (++next)->first-begintime; 

        int added=0, total=0;
        for( ; mit!=history.end(); ++mit) {
            const GPS::POINTINFO& pt = mit->second;
            double t = mit->first;
            if( t < tstart) continue;
            if( t > tstop) break;
            total++;
            if( avoid_saa && astro::EarthCoordinate(pt.lat, pt.lon).insideSAA()) continue;
            added++;
            exp.fill( pt.dirZ, deltat);
        }

        m_f.info() << "Number of steps added: " << added << ", rejected in SAA: "<< (total-added) << std::endl;
        m_f.info() << "Total elapsed time: " << deltat*total << " seconds." << std::endl;
        return;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void run()
    {
        m_f.setMethod("run()");

        // create the differential exposure object
        Exposure ex( m_pars["pixelsize"], m_pars["binsize"]);
#if 0
        m_f.info() << "Creating an exposure object from a pointing history file ...";

        LoadExposureFromGlast(  m_pars, ex); 
#else
        m_f.info() << "Create exposure object from simple point" << std::endl;
        ex.fill(astro::SkyDir(0,0), 1.0);
        
#endif
        // create the fits output file from the Exposure file
        m_f.info() 
            << "writing out the differential exposure file to "
            << m_pars.outputFile() << std::endl;
        map_tools::HealpixArrayIO::instance().write(ex.data(), m_pars.outputFile(), m_pars.table_name());

 
    }
private:
    Parameters m_pars;
    st_stream::StreamFormatter m_f;

};
// Factory which can create an instance of the class above.
st_app::StAppFactory<ExposureCubeApp> g_factory("exposure_cube");

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
