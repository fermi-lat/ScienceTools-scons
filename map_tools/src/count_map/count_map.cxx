/** @file count_map.cxx
@brief build the count_map application

$Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "tuple/ITable.h"
#include "astro/SkyDir.h"

#include "image/Image.h" 
#include "st_app/IApp.h"

#include <algorithm>
using namespace map_tools;

/** @class CountMap 
@brief The count_map application

*/
class CountMap : public st_app::IApp {
public:
    CountMap():st_app::IApp("count_map"){}

    void process( const MapParameters& pars,  tuple::ITable & t,  SkyImage& image)
    {
        const double&  ra = t.selectColumn(pars.raName());
        const double& dec = t.selectColumn(pars.decName());

        for( tuple::Iterator it = t.begin(); it !=t.end(); ++it){
            image.addPoint(astro::SkyDir(ra, dec), 1.0); 
        }
    }


    void run(){
        // read in, or prompt for, all necessary parameters
        MapParameters pars(hoopsGetParGroup());

        // connect to  input data, specifying filter
        tuple::ITable::Factory& tableFactory = * tuple::ITable::Factory::instance();
        tuple::ITable& table = *tableFactory( pars.inputFile() , "", pars.filter());

        // create the image object
        SkyImage image(pars);

        // process the table and image
        process(pars, table, image);

        std::cout << "Total added to image: " << image.total() 
            <<" at file\n\t" << pars.outputFile() << std::endl;
    }

} app;
