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
   /** \brief Create AppExample2 object, performing initializations needed for running the application.
    */
    ExposureCubeApp(): st_app::StApp(), m_par_group(st_app::StApp::getParGroup("exposure_cube")) {
      // Prompt for all parameters.
      m_par_group.Prompt();

      // Save the values just prompted for.
      m_par_group.Save();
    }
    //--------------------------------------------------------------------------
    void LoadExposureFromGlast( const MapParameters& pars,   Exposure& exp )
    {
        using tip::Table;

        double 
            tstart = pars["tstart"], 
            tstop = pars["tstop"];

        // connect to  input data
        const Table & table = *tip::IFileSvc::getSvc().readTable(pars.inputFile(), "Ext1");

        for (Table::ConstIterator it = table.begin(); it != table.end(); ++it) {

            const Table::Record & record = *it;

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
        // read in, or prompt for, all necessary parameters
        MapParameters pars(m_par_group);

        // create the exposure, and fill it from the history file
        Exposure ex( pars["pixelsize"], pars["costhetabinsize"]);

        LoadExposureFromGlast(  pars, ex); 

        // create the fits output file from the Exposure file
        ExposureHyperCube cube(ex, pars.outputFile());

    }
    private:
    st_app::AppParGroup & m_par_group;

};
// Factory which can create an instance of the class above.
st_app::StAppFactory<ExposureCubeApp> g_factory;
