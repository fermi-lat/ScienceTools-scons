/** @file exposure_map.cxx
    @brief build the exposure_map application

     @author Toby Burnett

     $Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/Parameters.h"
#include "astro/SkyDir.h"
#include <iostream>

using namespace map_tools;
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ReadPars : public Parameters{
public:
    ReadPars(int argc, char * argv[]): Parameters(argc,argv){
        getDouble("ra", 0);
        getDouble("dec", 0);
    }
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char * argv[]) {
    try{

        // read in, or prompt for, all necessary parameters
        ReadPars pars(argc, argv);

        SkyImage image(pars.inputFile(), "Primary"); 

        std::cout << "Value of pixel at ra, dec: "<< pars["ra"]<< ", "<< pars["dec"] << ": "
        <<image.pixelValue(astro::SkyDir(pars["ra"],pars["dec"],astro::SkyDir::GALACTIC)) << std::endl;

    }catch( const std::exception& e){
        std::cerr << "caught exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
