/** @file count_map.cxx
    @brief build the count_map application

    $Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "tuple/ITable.h"
#include "astro/SkyDir.h"

#include "image/Image.h" 

#include <algorithm>
namespace cmap { // for count_map helper classes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class AddCount
@brief Function object to apply to a tuple. fill the image with counts of (ra, dec) pairs
*/
class AddCount { 
public:

    AddCount( const MapParameters& pars, tuple::ITable & t, SkyImage& image)
        : m_image(image)
    {
      t.selectColumn(pars.raName());
      t.selectColumn(pars.decName());
    }

    void operator()(std::vector<double> tuple){
        double ra = tuple[0], dec=tuple[1], intensity=1.0; 
        m_image.addPoint(astro::SkyDir(ra, dec), intensity); 
    }

private:
    SkyImage& m_image;
};
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char * argv[]) {
    using namespace cmap;
    try{

        // read in, or prompt for, all necessary parameters
        MapParameters pars(argc, argv);

        // connect to  input data, specifying filter
        tuple::ITable::Factory& tableFactory = * tuple::ITable::Factory::instance();
        tuple::ITable& table = *tableFactory( pars.eventFile() , "", pars.filter());

        // create the image object
        SkyImage image(pars);

        // define a function opject to analyze each row, then apply it to all selected rows
        AddCount count(pars, table, image);
        std::for_each( table.begin(), table.end(), count);

        std::cout << "Total added to image: " << image.total() << std::endl;
 
    }catch( const std::exception& e){
        std::cerr << "caught exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

