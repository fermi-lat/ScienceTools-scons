/** @file read_map.cxx
    @brief build the read_map application

     @author Toby Burnett

     $Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"
#include "astro/SkyFunction.h"
#include "astro/SkyDir.h"
#include <iostream>

using namespace map_tools;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char * argv[]) {
    int rc = 0;
    try{ 
  
       // read in, or prompt for, all necessary parameters
        MapParameters pars(argc, argv);

        std::cout << "Reading FITS input file " << pars.inputFile() << std::endl;
        SkyImage image(pars.inputFile(), pars.tableName()); 

        std::cout << "Value of pixel at l, b: "<< pars["ra"]<< ", "<< pars["dec"] << ": "
        <<image(astro::SkyDir(pars["ra"],pars["dec"],astro::SkyDir::GALACTIC)) 
        << std::endl;

        std::cout << "Creating copy at file " << pars.outputFile() << std::endl;
        SkyImage copy(pars);
        copy.fill(image);

    }catch( const std::exception& e){
        std::cerr << "caught exception: " << e.what() << std::endl;
        rc=1;
    }
    return rc;
}

/** @page map_stats_guide map_stats users's Guide


 @verbinclude read_map.par

*/
