/** @file exposure_map.cxx
@brief Classes specific to the exposure_map application

@author Toby Burnett

See the <a href="exposure_map_guide.html"> user's guide </a>.

$Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"
#include "map_tools/Exposure.h"

#include "irfInterface/IAeff.h"
#include "irfInterface/Irfs.h"
#include "irfInterface/IrfsFactory.h"
#include "dc1Response/loadIrfs.h"

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/AppParGroup.h"
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"



#include "astro/SkyDir.h"

#include <stdexcept>
using namespace map_tools;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class IrfAeff
@brief function class implements effective area, as adapter to irfInterface::IAeff
*/
class IrfAeff : public Exposure::Aeff{
public:
    /**
    @param aeff an object from the CALDB stuff. If zero, implement linear 
    @param energy energy to evaluate
    @param cutoff limit for cos(theta)
    */
    IrfAeff(const irfInterface::IAeff* aeff, double energy, double cutoff=0.25)
        :m_aeff(aeff),m_energy(energy), m_cutoff(cutoff)
    {}

    double operator()(double costh) const
    {
        if( m_aeff==0 ){
            return costh<m_cutoff? 0 : (costh-m_cutoff)/(1.-m_cutoff);
        }

        return m_aeff->value(m_energy, acos(costh)*180/M_PI, 0);
    }
    const irfInterface::IAeff* m_aeff;
    double m_energy;
    double m_cutoff;
};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class RequestExposure 
@brief function class requests a point from the exposure
*/
class RequestExposure : public astro::SkyFunction
{
public:
    RequestExposure(const Exposure& exp, const Exposure::Aeff& aeff, double norm=1.0)
        : m_exp(exp)
        , m_aeff(aeff)
        , m_norm(norm)
    {}
    double operator()(const astro::SkyDir& s)const{
        return m_norm*m_exp(s, m_aeff);
    }
private:
    const Exposure& m_exp;
    const Exposure::Aeff& m_aeff;
    double m_norm;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class ExposureMapApp
@brief the exposure_map application class

*/
class ExposureMapApp : public  st_app::StApp  {
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /** \brief Create an application object, performing initializations needed for running the application.
    */
    ExposureMapApp()
        : st_app::StApp()
        , m_f("ExposureMapApp", "", 2)
        , m_pars(st_app::StApp::getParGroup("exposure_map")) 
    {
    }
        ~ExposureMapApp() throw() {} // required by StApp with gcc

    /**
    */

    irfInterface::IAeff* findAeff(std::string rspfunc)
    {
        using namespace irfInterface;
        m_f.setMethod("findAeff");

        class AeffSum : public irfInterface::IAeff {
        public:  
            AeffSum(){
                irfInterface::Irfs* dc1 = IrfsFactory::instance()->create("DC1::Front");
                m_front = dc1->aeff();
                dc1 = IrfsFactory::instance()->create("DC1::Back");
                m_back = dc1->aeff();
            }

            virtual double value(double energy, 
                const astro::SkyDir &srcDir, 
                const astro::SkyDir &scZAxis,
                const astro::SkyDir &scXAxis) const 
            {return 0;}

            virtual double value(double energy, double theta, double phi) const 
            {
                return m_front->value(energy, theta, phi) + m_back->value(energy, theta, phi);
            }
            virtual IAeff * clone(){throw std::runtime_error("clone?"); return 0;};

        private:
            const irfInterface::IAeff* m_front;
            const irfInterface::IAeff* m_back;
        };


        // set up irf stuff, and translate the IRF name        
        dc1Response::loadIrfs();

        m_f.info() << "Using Aeff " ;
        std::string irfname;
        if(      rspfunc=="DC1F") irfname="DC1::Front";
        else if( rspfunc=="DC1B") irfname="DC1::Back";
        else if( rspfunc=="DC1FB") {
            // special case handled by custom class above.
            m_f.info() << "DC1::Front + DC1::Back" << std::endl;
            return new AeffSum();
        }else if( rspfunc=="SIMPLE" ){
            m_f.info() << "Simple linear form " << std::endl;
            return 0;
        }else { 
            throw std::invalid_argument(
                std::string("Response function not implemented here: "+rspfunc));
        }
        m_f.info() << irfname << std::endl;
        Irfs* dc1 = IrfsFactory::instance()->create(irfname);
        return dc1->aeff();

    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void run() {

        m_f.setMethod("run()");

        // create the exposure, read it in from the FITS input file
        m_f.info() << "Creating an Exposure object from file " << m_pars.inputFile() << std::endl;

        Exposure ex(m_pars.inputFile() ); 
        double total_elaspsed = ex.total();
        m_f.info() << "\ttotal elapsed time: " << total_elaspsed << std::endl;

        irfInterface::IAeff* aeff = findAeff(m_pars.getValue<std::string>("rspfunc"));

        // create the image object, fill it from the exposure, write out
        std::clog << "Creating an Image, will write to file " << m_pars.outputFile() << std::endl;
        SkyImage image(m_pars); 
        double ratio = sqrt(10.),emin= 100/ratio, emax = 1e5;
        int layer = 0;
        double energy = m_pars["emin"], eratio = m_pars["eratio"];
        int layers = m_pars.getValue<int>("layers");
        for (int layer=0; layer <layers; energy *= eratio, ++layer){

            double norm = aeff!=0? aeff->value(energy,0,0): 1.0; // for normalization
            std::clog << "Generating layer " << layer
                << " at energy " << energy << " MeV " 
                << " Aeff(0): " << norm << " cm^2"<< std::endl;

            RequestExposure req(ex, IrfAeff(aeff, energy), 1.); 
            image.fill(req, layer);
        }
    }

private:
    MapParameters m_pars;
    st_stream::StreamFormatter m_f;

};
// Factory which can create an instance of the class above.
st_app::StAppFactory<ExposureMapApp> g_factory("exposure_map");

/** @page exposure_map_guide exposure_map users's Guide

 - Input: an exposure cube FITS file and an effective area function.
 - Output: an image FITS file with with multiple layers for different energies

 This application  reads an exposure cube, as generated by the 
 <a href="exposure_cube_guide.html">exposure_cube</a> application. The third dimension is bins
 in theta.
 
  It creates an exposure map, or maps, for a specific effective area function, defined by the 
  "rspfunc" parameter. Values are
  @param SIMPLE - simple linear function
  @param DC1F - DC1 parametrization for the front
  @param DC1B - DC1 parametraizatio for the back
  @param DC1FB - sum o the above

  The parameters describing the output image are 
  @param pixelsize degrees per pixel
  @param npix  number of pixels across
  @param projtype CAR for cartesian, AIT for Hammer-Aitoff, etc.


- The parameter file
 @verbinclude exposure_map.par

*/
