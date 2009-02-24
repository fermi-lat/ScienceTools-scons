/** @file cube_display.cxx
@brief Classes specific to the gtdispcube application

@author Toby Burnett

See the <a href="gtdispcube_guide.html"> user's guide </a>.

$Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/Exposure.h"
#include "healpix/CosineBinner.h"

#include "astro/SkyDir.h"

#include "st_app/StApp.h"
#include "st_app/StAppFactory.h"
#include "st_app/AppParGroup.h"
#include "st_stream/StreamFormatter.h"
#include "st_stream/st_stream.h"

#include "tip/IFileSvc.h"
#include "tip/Table.h"
#include "facilities/Util.h"

#include <sstream>
#include <iterator> // for ostream_iterator
#include <fstream>


#include <stdexcept>

namespace {
}

using namespace map_tools;
using healpix::CosineBinner;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class RequestExposure 
@brief function class requests a point from the exposure
*/
template< class F>
class RequestExposure : public astro::SkyFunction
{
public:
    RequestExposure(const Exposure& exp, const F& aeff, double norm=1.0)
        : m_exp(exp)
        , m_aeff(aeff)
        , m_norm(norm)
    {}
    double operator()(const astro::SkyDir& s)const{
        return m_norm*m_exp(s, m_aeff);
    }
private:
    const Exposure& m_exp;
    const F& m_aeff;
    double m_norm;
};
//---------- functor that picks an angle -------------------------
class SelectAngle {
public:
    SelectAngle(double costh): m_costh(costh){}
    
    double operator()(double z)const{
        return ( fabs(z-m_costh)<0.0001 )? 1 : 0;
    }
private:
    double m_costh;
};

// version for phi dependence.
class SelectAngles {
public:
    SelectAngles(double costh, double phi): m_costh(costh), m_phi(phi){}
    
    double integral(double z, double phi)const{
        return ( fabs(z-m_costh)<0.0001&& fabs(phi-m_phi)<0.1 )? 1 : 0;
    }
private:
    double m_costh, m_phi;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/** @class CubeDisplayApp
@brief the cube_display application class

*/
class CubeDisplayApp : public  st_app::StApp  {
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /** \brief Create an application object, performing initializations needed for running the application.
    */
    CubeDisplayApp()
        : st_app::StApp()
        , m_f("CubeDisplayApp", "", 2)
        , m_pars(st_app::StApp::getParGroup("gtdispcube")) 
    {
    }
        ~CubeDisplayApp() throw() {} // required by StApp with gcc


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void run() {

        m_f.setMethod("run()");

        std::string infile(m_pars["infile"].Value())
            , outfile(m_pars["outfile"].Value())
            , table( m_pars["table"].Value());

        // create the exposure, read it in from the FITS input file
        facilities::Util::expandEnvVar(&infile);
        facilities::Util::expandEnvVar(&outfile);
        m_f.info() << "Creating an Exposure object from file " << infile << std::endl;

        Exposure ex(infile, table);
#if 0 //! todo: find out how this was broken
        double total_elaspsed = ex.total();
        m_f.info() << "\ttotal elapsed time: " << total_elaspsed << std::endl;
#endif

        // analyze the cos(theta) bins by creating a binner object, assuming that the binning
        // parameters were set when the Exposure object was read in
        CosineBinner bins;
        std::vector<float> costhbins;
        for( CosineBinner::iterator it = bins.begin(); it<bins.end_costh(); ++it){
            costhbins.push_back( bins.costheta(it));
        }
        int layers(costhbins.size()); 

        // create the image object with a layer for each bin, fill it from the exposure, write out
        std::clog << "Creating an Image, will write to file " << outfile<< std::endl;

        // extract info for image from standard pars
        double xref(m_pars["xref"]), 
               yref(m_pars["yref"]), 
               pixscale(m_pars["pixscale"]); 
        std::string coordsys(m_pars["coordsys"].Value()), proj(m_pars["proj"].Value());
        bool galactic (coordsys=="GAL");
        int numxpix(m_pars["nxpix"]), 
            numypix(m_pars["nypix"]);
        double fov = numxpix==1? 180. : numxpix*pixscale;

        astro::SkyDir center(xref, yref, galactic?  astro::SkyDir::GALACTIC : astro::SkyDir::EQUATORIAL);
#if 0       
        SkyImage image (center, outfile, pixscale, fov, layers, proj, galactic);

        for ( std::vector<double>::size_type layer = 0; layer != layers; ++layer){
            double costh ( costhbins[layer] );
            std::clog << "Generating layer " << layer << ", theta =  " << acos(costh)*180/M_PI << std::endl;

            // set phi negative to flag average
            RequestExposure<SelectAngle> req(ex, SelectAngle(costh) ); 
            image.fill(req, layer);
        }

#else
        // make a table of costh, phi
        std::ofstream out("test_table.txt");
        for ( std::vector<double>::size_type layer = 0; layer != layers; ++layer){
            double costh ( costhbins[layer] );
            out <<"\n"<< ex(center, SelectAngle(costh)) ;
            for ( std::vector<double>::size_type philayer = 0; philayer != CosineBinner::nphibins(); ++philayer){
                double phi ( 3*philayer*M_PI/180);

                out << "\t " << ex.integral(center, SelectAngles(costh, phi)) ;
                //std::clog << "Generating layer " << layer << ", theta =  " 
                //    << acos(costh)*180/M_PI <<", phi = "<< phi*180/M_PI << std::endl;
                //RequestExposure<SelectAngles> req(ex, SelectAngles(costh,phi) );
                //image.fill(req, layer);
            }
        }
        out << std::endl;
       
#endif
    }


private:
    st_stream::StreamFormatter m_f;
    st_app::AppParGroup& m_pars;
};
// Factory which can create an instance of the class above.
st_app::StAppFactory<CubeDisplayApp> g_factory("gtdispcube");

/** @page gtdispcube_guide gtdispcube users's Guide

 - Input: an exposure cube FITS file and an effective area function.
 - Output: an image FITS file with with multiple layers for each bin in the angular distribution from that point

 This application  reads an exposure cube, as generated by the 
 <a href="exposure_cube_guide.html">exposure_cube</a> application. The third dimension is bins
 in theta.
 
  It creates a FITS multilayer image.

  The parameters describing the output image are 
  @param pixelsize degrees per pixel
  @param numxpix  number of pixels across: if 1, generate full sky 
  @param numypix  number of vertical pixels: leave at 1 for full sky, or square
  @param projtype CAR for cartesian, AIT for Hammer-Aitoff, etc.


- The parameter file
 @verbinclude cube_display.par
  Print Version 
cube_display
Generates an exposure map, or a set of exposure maps for different energies, from a livetime cube written by gtlivetimecube. 
@verbatim
Prerequisites
Input Files: 
Livetime or Exposure cube FITS file from gtlivetimecube 
Links: 

Basic FTOOL Parameter Usage 
General Parameters
  infile [file]  
    Exposure or Livetime cube input file.  
      
  outfile [file] 
    Exposure map output file name (FITS format  image). 
          
  numxpix = 1 [int] 
    Size of the X axis in pixels. Default (1) for full sky
    
  numypix = 1 [int] 
    Size of the Y axis in pixels. Default (1) to copy numxpix, or full sky
    
  pixscale = 1.0 [float]  
    Image scale (in degrees/pixel).  
    
  coordsys = "CEL" [string] 
    Coordinate system, CEL or GAL. 
    
  xref = 0. [float] 
    First coordinate of image center in degrees (RA or Galactic l). (default 0) 
    
  yref = 0. [float]  
    Second coordinate of image center in degrees (DEC or Galactic b). (default 0)
    
  axisrot=0. [float] 
    Rotation angle of image axis, in degrees. (default 0)
    
  (proj = "AIT") [string] 
    Coordinate projection (AIT|ZEA|ARC|CAR|GLS|MER|NCP|SIN|STG|TAN); see Calabretta & Greisen 2002, A&A, 395, 1077 for definitions of the projections.  
    Must be AIT, ZEA or CAR for auto full sky. 

  (filter  = no default) [string] 
    Filter expression (FTOOLS style).  
    
  (table = "Exposure")  
    Exposure cube extension. 
    
  (chatter = 2) [int] 
    Chattiness of output. 
    
  (clobber = "yes") [boolean]  
    Overwrite existing output files with new output files. 
    
  (debug = "no") [boolean] 
    Debugging mode activated. 
    
  (gui = "no") [boolean]  
    Gui mode activated. 
    
  (mode = "ql") [string] 
    Mode of automatic parameters. 

Also See
gtlivetimecube 
Basic FTOOL Parameter Usage 

--------------------------------------------------------------------------------

 

Owned by: Toby Burnett  

Generated on: Mar 12 21:53:43 2006 

Last updated by: Toby Burnett Back to Top  

 @endverbatim
 

 

 

 
 

 

*/
