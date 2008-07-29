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
        std::string infile(pars["infile"].Value()), 
            outfile(pars["outfile"].Value()), table(pars["table"].Value());

        std::cout << "Reading FITS input file " << infile << std::endl;
        SkyImage image(infile, table); 
        int layers (image.layers()); // default will copy each layer

        std::cout << "Creating copy at file " << outfile << std::endl;
        // extract info for image from standard pars
        double xref(pars["xref"]), 
               yref(pars["yref"]), 
               pixscale(pars["pixscale"]); 
        std::string coordsys(pars["coordsys"].Value()), proj(pars["proj"].Value());
        bool galactic (coordsys=="GAL");
        int numxpix(pars["numxpix"]), 
            numypix(pars["numypix"]);
        double fov = numxpix==1? 180. : numxpix*pixscale;

        astro::SkyDir center(xref, yref, galactic?  astro::SkyDir::GALACTIC : astro::SkyDir::EQUATORIAL);
        SkyImage copy (center, outfile, pixscale, fov, layers, proj, galactic);
        for( int ilayer = 0; ilayer< layers; ++ilayer){
            std::cout << "copying layer " << ilayer << std::endl;
            image.setLayer(ilayer); // extract only this layer
            copy.fill(image, ilayer);  // and add to the same one in the copy
        }

    }catch( const std::exception& e){
        std::cerr << "caught exception: " << e.what() << std::endl;
        rc=1;
    }
    return rc;
}

/** @page map_stats_guide map_stats users's Guide


 @verbinclude read_map.par

*/
