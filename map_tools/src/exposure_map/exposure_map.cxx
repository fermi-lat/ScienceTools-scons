/** @file exposure_map.cxx

@brief build the exposure_map application
 $Header$

*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "map_tools/Exposure.h"
#include "map_tools/ExposureHyperCube.h"

#include "table/FitsService.h"
#include "table/Constants.h"
#include "astro/SkyDir.h"
#include "table/PrimaryHDU.h"
#include <algorithm>
#include <fstream>

namespace { // for simple helper classes
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class MyCube
    @ brief setup an exposure hypercube from a FITS file
    */
class MyCube : public table::PrimaryHDU<float>{
public:
    MyCube(const table::FitsService * iosrv): m_total(0){

        readHeaderKeys(iosrv);
        readAllKeys(iosrv);
        int size = m_naxes[0]*m_naxes[1]*m_naxes[2];
        Primary::setBufferSize(size);

        m_data.resize(size);
        this->read(iosrv, 1, size);
        std::string comment;
        iosrv->readKey("TOTAL", m_total,  comment);

    }
    float total()const{return m_total;};
private:
    float m_total;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class Aeff
    @ brief function class implements effective area
    */
class Aeff : public Exposure::Aeff{
public:
    Aeff(double cutoff=0.25):m_cutoff(cutoff){}
    double operator()(double costh) const
    {
        if(m_cutoff==2.) return 1.0;
        return costh<m_cutoff? 0 : (costh-m_cutoff)/(1.-m_cutoff);
    }
    double m_cutoff;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class RequestExposure 
    @brief function class requests a point from the exposure
*/
class RequestExposure : public SkyImage::Requester
{
public:
    RequestExposure(const Exposure& exp, const Aeff& aeff, double norm=1.0)
        : m_exp(exp)
        , m_aeff(aeff)
        , m_norm(norm)
    {}
        float operator()(const astro::SkyDir& s)const{
            return m_norm*m_exp(s, m_aeff);
        }
private:
    const Exposure& m_exp;
    const Aeff& m_aeff;
    double m_norm;
};
} // anonymous namespace
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int main(int argc, char * argv[]) {
using namespace table;
    try{

        // read in, or prompt for, all necessary parameters
        MapParameters pars(argc, argv);

        std::vector<std::string> files; files.push_back(pars.inputFile());
        table::FitsService iosrv(files);

        MyCube cube(& iosrv);
        
        iosrv.createNewFile(pars.outputFile(), pars.templateFile());

        // create the exposure, read it in from the FITS input file
        Exposure ex(cube.image(), cube.total()); 

        // create the image object, fill it from the exposure, write out
        SkyImage image(pars); 

        image.fill(RequestExposure(ex, Aeff(), 1.0));
        image.write(&iosrv);

        iosrv.closeFile();
    }catch( const std::exception& e){
        std::cerr << "caught exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
