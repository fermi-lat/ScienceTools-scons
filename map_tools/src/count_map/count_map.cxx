/** @file count_map.cxx
@brief build the count_map application

$Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "tip/Table.h"
#include "tip/IFileSvc.h"
#include "astro/SkyDir.h"

#include "image/Image.h" 
#include "st_app/AppParGroup.h"
#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"

#include <algorithm>
using namespace map_tools;

/** @class CountMap 
@brief The count_map application
*/
class CountMap : public st_app::StApp {
public:

    CountMap()
        : st_app::StApp()
        , m_par_group(st_app::StApp::getParGroup("count_map")) 
    {
        // Prompt for all parameters.
        m_par_group.Prompt();

        // Save the values just prompted for.
        m_par_group.Save();
    }


    void run(){
        using tip::Table;
        // read in, or prompt for, all necessary parameters
        MapParameters pars( m_par_group );

        // connect to  input data, specifying filter
        const Table & table = *tip::IFileSvc::instance().readTable(pars.inputFile(), pars.table_name(), pars.filter() );
        if( pars.chatter()>0) {
            std::cout << "Reading file " << pars.inputFile() ;
            if( ! pars.filter().empty() ) std::cout << "\n\tfiltered by " << pars.filter() ;
            std::cout<< std::endl;
        }

        // create the image object
        SkyImage image(pars);

        for (Table::ConstIterator it = table.begin(); it != table.end(); ++it) {

            // Create local reference to the record to which the iterator refers:
            const Table::Record & record = *it;
            // Get the current values
            double ra = record[pars.raName()].get(), 
                dec=record[pars.decName()].get();

            image.addPoint(astro::SkyDir(ra, dec) );
        }
        if( pars.chatter()>0) {
            std::cout << "Total added to image: " << image.total() 
                <<" at file\n\t" << pars.outputFile() << std::endl; }
    }
private:
    st_app::AppParGroup & m_par_group;
};

st_app::StAppFactory<CountMap> g_factory;
