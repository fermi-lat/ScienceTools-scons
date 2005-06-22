/** @file read_map.cxx
    @brief build the map_stats application

     @author Toby Burnett

     $Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/Parameters.h"
#include "astro/SkyStat.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/AppParGroup.h"
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"


#include <iostream>
#include <iomanip>

using namespace map_tools;

/** @class MapStats
    @brief  A simple application that reads an image from a FITS file, and computes statistics from it.


    */
class MapStats : public st_app::StApp {
public:
    MapStats()
        : st_app::StApp()
        , m_pars(st_app::StApp::getParGroup("map_stats")) 
        , m_f("MapStats", "", 2)
    {
    }
    
    virtual void run()
    {
      // For output streams, set name of method, which will be used in messages when tool is run in debug mode.
      m_f.setMethod("run()");


        m_f.out()  
            << "Anaysis of the FITS image file " << m_pars.inputFile() << std::endl;
        SkyImage image(m_pars.inputFile(), m_pars.table_name() );

        // pass the image to the SkyStat guy
        astro::SkyStat ss(image, 128); 
        m_f.out() 
#if 0 //def WIN32
            << std::setw(12) << "average: " << ss.ave() << std::endl
            << std::setw(12) <<   "sigma: " << ss.sigma() << std::endl
            << std::setw(12) << "minimum: " << ss.min() << std::endl
            << std::setw(12) << "maximum: " << ss.max() << std::endl;
#else // since gcc can't handle iomanips?
            << "\t" << "average: " << ss.ave() << std::endl
            << "\t" << "  sigma: " << ss.sigma() << std::endl
            << "\t" << "minimum: " << ss.min() << std::endl
            << "\t" << "maximum: " << ss.max() << std::endl;
#endif
        if( ss.rejected()>0){
            m_f.warn() 
                << " rejected  " << ss.rejected() << " points in average." << std::endl;
        }

    }
private:
    Parameters m_pars;
    st_stream::StreamFormatter m_f;

};
// Factory that creates an instance of the class above
st_app::StAppFactory<MapStats> g_factory("map_stats");

/** @page map_stats_guide map_stats users's Guide

 A simple application that reads an image from a FITS file, and computes statistics from it.
 Code is in map_stats.cxx.

 Example output:

 @verbatum
 Reading FITS input file D:/Users/burnett/ST_dev/map_tools/v5r5p2/output/exp_1_day_ait.fits
           average: 11585.8
             sigma: 7507.91
           minimum: 13909
           maximum: 19200.6
          rejected: 1
 @endverbatim

 @verbinclude map_stats.par

*/
