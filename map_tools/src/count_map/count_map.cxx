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
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"
#include "TSystem.h" // ROOT, for gSystem
#include <algorithm>
using namespace map_tools;

/** @class CountMapApp 
@brief The count_map application
*/
class CountMapApp : public st_app::StApp {
public:

    /** @brief ctor sets up parameter object

    Note that Parameters will prompt and save when created
    */
    CountMapApp()
        : st_app::StApp()
        , m_f("CountMapApp", "", 2)
        , m_pars( st_app::StApp::getParGroup("count_map")) 
    { 
#ifdef WIN32 // needed for ROOT on windows
        gSystem->Load("libTree.dll");
//        gSystem->Load("libHist.dll");
#endif
    }
    ~CountMapApp() throw() {} // required by StApp with gcc

    void run(){
        using tip::Table;
        // For output streams, set name of method, which will be used in messages when tool is run in debug mode.
        m_f.setMethod("run()");


        // connect to  input data, specifying filter
        const Table & table = *tip::IFileSvc::instance().readTable(m_pars.inputFile(), m_pars.table_name(), m_pars.filter() );
        m_f.info() << "Reading file " << m_pars.inputFile() ;
        if( ! m_pars.filter().empty() ) m_f.out() << "\n\tfiltered by " << m_pars.filter() ;
        m_f.info() << "\n\tevents: " << table.getNumRecords() << std::endl;

 
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
        m_f.info() << "Total added to image: " << image.total() 
                <<" at file\n\t" << m_pars.outputFile() << std::endl; 
    }
private:
    st_stream::StreamFormatter m_f;
    MapParameters m_pars;

};

st_app::StAppFactory<CountMapApp> g_factory("count_map");
