/** @file exposure_map.cxx
@brief build the exposure_map application

@author Toby Burnett

$Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"
#include "map_tools/Exposure.h"
#include "st_app/IApp.h"
#include "astro/SkyDir.h"
using namespace map_tools;

/** @class ExposureMapApp
@brief the exposure_map application class

*/

class ExposureMapApp : public st_app::IApp {
public:
    ExposureMapApp():st_app::IApp("exposure_map"){}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /** @class Aeff
    @brief function class implements effective area
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
    class RequestExposure : public astro::SkyFunction
    {
    public:
        RequestExposure(const Exposure& exp, const Aeff& aeff, double norm=1.0)
            : m_exp(exp)
            , m_aeff(aeff)
            , m_norm(norm)
        {}
        double operator()(const astro::SkyDir& s)const{
            return m_norm*m_exp(s, m_aeff);
        }
    private:
        const Exposure& m_exp;
        const Aeff& m_aeff;
        double m_norm;
    };
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    void run() {
        // read in, or prompt for, all necessary parameters
        MapParameters pars( IApp::hoopsGetParGroup());

        // create the exposure, read it in from the FITS input file
        Exposure ex(pars.inputFile() ); 

        // create the image object, fill it from the exposure, write out
        SkyImage image(pars); 

        RequestExposure req(ex, Aeff(), 1.0);
        image.fill(req);

    }

} application;