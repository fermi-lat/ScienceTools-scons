/** @file exposure_cube.cxx
@brief build the exposure_cube application

@author Toby Burnett
$Header$
*/

#include "hoops/hoops_prompt_group.h"
#include "map_tools/Exposure.h"
#include "healpix/HealpixArrayIO.h"

#include "astro/SkyDir.h"
#include "astro/GPS.h"
#include "astro/EarthCoordinate.h"

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/AppParGroup.h"

#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"
#include "tip/IFileSvc.h"
#include "tip/Table.h"

#include <iostream>
#include <stdexcept>
using namespace map_tools;
using healpix::HealpixArrayIO;


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
    void loadExposureWithGPS(Exposure& exp, const std::string& inputFile, const Exposure::GTIvector& gti )
    {
        using astro::GPS;
        double 
            tstart ( gti.front().first ),
            tstop  ( gti.front().second),
            zmin ( m_pars["zmin"] );

#if 1
        throw std::invalid_argument("exposure_cube: text file not currently implemented");
#else // fix this if we still need to extract exposure cubes from text files
        // read from text or FITS file here
        GPS& gps = *GPS::instance();
        gps.setPointingHistoryFile(inputFile);
        const std::map<double,GPS::POINTINFO>& history = gps.getHistory();
        GPS::history_iterator mit = history.begin(), next=mit;
        double begintime=mit->first;
        // 2/8/2006 JP commented out the following line to silence compiler warning.
        //double endtime = (--(history.end()))->first;

        double deltat = (++next)->first-begintime; 

        int added=0, total=0;
        for( ; mit!=history.end(); ++mit) {
            const GPS::POINTINFO& pt = mit->second;
            double t = mit->first;
            if( t < tstart) continue;
            if( t > tstop) break;
            total++;
//            if( avoid_saa && astro::EarthCoordinate(pt.lat, pt.lon).insideSAA()) continue;
            added++;
	    astro::SkyDir dirZenith(pt.position.unit());
            exp.fill( pt.dirZ, dirZenith, deltat, zmin); // zcut of -1: no cut.
        }

        m_f.info() << "Number of steps added: " << added << ", rejected: "<< (total-added) << std::endl;
        m_f.info() << "Total elapsed time: " << deltat*total << " seconds." << std::endl;
#endif
        return;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void run()
    {
        m_f.setMethod("run()");

        prompt();


        // create the differential exposure object
	double pixelsize(m_pars["pixelsize"]), binsize(m_pars["binsize"]);
        double phibins(m_pars["phibins"]);
		
        double tstart(m_pars["tstart"]),
               tstop(m_pars["tstop"]),
               zmin ( m_pars["zmin"] );

        // note that the phi binning option is turned on by setting the static parameter
        if( phibins>0) healpix::CosineBinner::setPhiBins(phibins); 

        Exposure ex( pixelsize, binsize, zmin);
        Exposure::GTIvector gti; 

        gti.push_back(std::make_pair(tstart,tstop));
        std::string infile(m_pars["infile"].Value()),
            outfile(m_pars["outfile"].Value()),
            table(m_pars["table"].Value()),
            outtable(m_pars["outtable"].Value());
        m_f.info() << "Creating an exposure object from a pointing history file ..." << infile << std::endl;
        m_f.info() << "\ttstart: " << tstart << "\n\t tstop: "<< tstop << std::endl;
        if( zmin>-1.){
            m_f.info() << "\t  zmin: "<< zmin << ", cut above horizon " << std::endl;
        }


        bool isText = infile.find(".txt") != std::string::npos;

        m_f.info() << "Opening " << (isText? "text":"FITS") << " format pointing history file " 
                << infile << std::endl;

        if( isText ) {
            loadExposureWithGPS(ex, infile, gti);
        }else{
            tip::Table * scData = tip::IFileSvc::instance().editTable(infile, table);
            ex.load(scData, gti);
        }

        // create the fits output file from the Exposure file
        m_f.info() 
            << "writing out the differential exposure file to "
            << outfile << ": added " << ex.total() << " seconds" << std::endl;
        if( zmin>-1){
            m_f.info() << " lost " << ex.lost() << " seconds from zcut" << std::endl;
        }
       HealpixArrayIO::instance().write(ex.data(), outfile, outtable);

 
    }
    void prompt() {
        m_pars.Prompt("outfile");
        m_pars.Prompt("infile");
        m_pars.Prompt("tstart");
        m_pars.Prompt("tstop");
        m_pars.Prompt("zmin");


        m_pars.Prompt("filter");
        m_pars.Prompt("table");
        m_pars.Prompt("chatter");
        m_pars.Prompt("clobber");
        m_pars.Prompt("debug");
        m_pars.Prompt("gui");
        m_pars.Save();
    }

private:
	hoops::ParPromptGroup m_pars;
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

-Output: a FITS table with HEALpix pixelization, defined by the parameter file, to be read by
the exposure_map utility.

@verbinclude exposure_cube.par

*/
