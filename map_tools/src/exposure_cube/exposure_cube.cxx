/** @file exposure_cube.cxx

@brief build the exposure_cube application
$Header$
*/

#include "map_tools/MapParameters.h"
#include "map_tools/Exposure.h"
#include "map_tools/ExposureHyperCube.h"

#include "tuple/ITable.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class ExposureMapParameters : public MapParameters 
{
public:
    ExposureMapParameters(int argc, char* argv[])
        : MapParameters(argc, argv)
        {
            getDouble("costhetabinsize", 0.025); // 40 bins in sqrt(1-cos(thetat))
            getDouble("skybinsize", 1.0); // 1 degree bins in ra and dec
            getDouble("tstart");
            getDouble("tstop");
        }
    };
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char * argv[]) {

    try{

        // read in, or prompt for, all necessary parameters
        ExposureMapParameters pars(argc, argv);

        // open output file
        table::FitsService iosrv;
        iosrv.createNewFile(pars.outputFile(), pars.templateFile());

        // create the exposure, and fill it from the history file
        Exposure ex( pars["skybinsize"], pars["costhetabinsize"]);

        tuple::ITable::Factory& factory = *tuple::ITable::Factory::instance();

        ex.load( *factory(pars.eventFile(),"Ext1"), pars["tstart"] , pars["tstop"]); 

        ExposureHyperCube cube(ex);
        cube.write(&iosrv);
        iosrv.closeFile();

    }catch( const std::exception& e){
        std::cerr << "Error: ending with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
