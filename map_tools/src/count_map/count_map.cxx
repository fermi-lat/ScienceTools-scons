/** @file count_map.cxx
@brief build the count_map application

$Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "tip/Table.h"
#include "tip/IFileSvc.h"
#include "astro/SkyDir.h"

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

    /** @brief ctor sets up parameter object

    Note that Parameters will prompt and save when created
    */
    CountMap()
        : st_app::StApp()
        , m_pars( st_app::StApp::getParGroup("count_map")) 
    { }

    ~CountMap() throw() {}  // needed since StApp has empty throw.

    void run(){
        using tip::Table;

        // connect to  input data, specifying filter
        const Table & table = *tip::IFileSvc::instance().readTable(m_pars.inputFile(), m_pars.table_name(), m_pars.filter() );
        if( m_pars.chatter()>0) {
            std::cout << "Reading file " << m_pars.inputFile() ;
            if( ! m_pars.filter().empty() ) std::cout << "\n\tfiltered by " << m_pars.filter() ;
            std::cout<< std::endl;
        }

        // create the image object
        SkyImage image(m_pars);

        for (Table::ConstIterator it = table.begin(); it != table.end(); ++it) {

            // Create local reference to the record to which the iterator refers:
            Table::ConstRecord & record = *it;
            // Get the current values
            double ra = record[m_pars.raName()].get(), 
                dec=record[m_pars.decName()].get();

            image.addPoint(astro::SkyDir(ra, dec) );
        }
        if( m_pars.chatter()>0) {
            std::cout << "Total added to image: " << image.total() 
                <<" at file\n\t" << m_pars.outputFile() << std::endl; }
    }
private:
    MapParameters m_pars;
};

st_app::StAppFactory<CountMap> g_factory;
