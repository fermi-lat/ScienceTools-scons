 /** @file SkyImage.cxx

@brief implement the class SkyImage

*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "astro/SkyDir.h"
#include "image/Image.h"

namespace {
    static unsigned long lnan[2]={0xffffffff, 0x7fffffff};
    static double& dnan = *( double* )lnan;
        //! @brief add a string or douuble key to the image 

    BaseImage* image;
    void setKey(std::string name, double value, std::string unit="", std::string comment=""){
        image->addAttribute(DoubleAttr(name, value, unit, comment)); }
    void setKey(std::string name, std::string value,std::string unit="", std::string comment="")
    {image->addAttribute(StringAttr(name, value,unit,comment)); }

}
using namespace map_tools;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::SkyImage(const map_tools::MapParameters& pars)
: m_naxis1(pars.npix())
, m_naxis2(pars.npixY())
, m_naxis3(1) // for future expansion
, m_total(0)
, m_image(0)
{
    using namespace astro;

    // set the skydir default projection
    astro::SkyDir::setProjection(
        pars.xref()*M_PI/180,  pars.yref()*M_PI/180, //value at reference            
        pars.projType(),  
        0.5*(m_naxis1),        0.5*(m_naxis2),   // reference in center of view
        -pars.imgSizeX()/float(m_naxis1),   
        pars.imgSizeY()/float(m_naxis2),    
        pars.rot()*M_PI/180,              
        pars.uselb()          
        );


    bool galactic= pars.uselb();

    // setup the image: it needs an axis dimension array and the file name to write to
    std::vector<long> naxes(3);
    naxes[0]=m_naxis1;
    naxes[1]=m_naxis2;
    naxes[2]=m_naxis3;
    m_image = new FloatImg("skymap", pars.outputFile(), naxes);
    m_pixelCount = m_image->pixelCount();

    // fill the boundaries with NaN
    if( pars.projType()=="AIT") clear();

    image=m_image; // set up the anonymous convenience functions

    setKey("TELESCOP", "GLAST");

    setKey("INSTRUME", "SIMULATION");

    setKey("DATE-OBS", "");
    setKey("DATE-END", "");
    setKey("EQUINOX", 2000.0,"","Equinox of RA & DEC specifications");

    setKey("CTYPE1", std::string(galactic?"GLON--":"RA--")+ pars.projType()
        ,"","RA---%%%, %%% represents the projection method such as AIT");
    setKey("CRPIX1",  SkyDir::s_refX+0.5,"","Reference pixel"); // note that FITS pixel reference is off by 0.5
    setKey("CRVAL1",  SkyDir::s_refRA, "", "RA at the reference pixel");
    setKey("CDELT1",  SkyDir::s_scaleX,"",
        "X-axis incr per pixel of physical coord at position of ref pixel(deg)");
    setKey("CUNIT1",  "deg", "", "Physical unit of X-axis");

    setKey("CTYPE2",  std::string(galactic?"GLAT--":"DEC--")+ pars.projType()
        ,"","DEC---%%%, %%% represents the projection method such as AIT");

    setKey("CRPIX2",  SkyDir::s_refY+0.5,"","Reference pixel");// note that FITS pixel reference is off by 0.5
    setKey("CRVAL2",  SkyDir::s_refDEC, "", "DEC at the reference pixel"); 
    setKey("CDELT2",  SkyDir::s_scaleY,"",
        "Y-axis incr per pixel of physical coord at position of ref pixel(deg)");
    setKey("CUNIT2",  "deg", "", "Physical unit of Y-axis");

    setKey("CROTA2",  0, "", "Image rotation (deg)");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::addPoint(const astro::SkyDir& dir, double delta, int layer){

    std::pair<double,double> p= dir.project();
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if(  k< m_pixelCount){
        reinterpret_cast<FloatImg*>(m_image)->data()[k]+=delta;
        m_total += delta;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::fill(const Requester& req)
{

    FloatImg* image =  dynamic_cast<FloatImg*>(m_image); 

    for( size_t k = 0; k< m_pixelCount; ++k){
        // determine the bin center
        float 
            x = static_cast<int>(k%m_naxis1)+0.5, 
            y = static_cast<int>(k/m_naxis1)+0.5;
        try{
            astro::SkyDir dir(x,y,astro::SkyDir::PROJECTION);
            double t= req(dir);
            image->data()[k] = t; 
            m_total += t;
        }catch(... ) { // any exception: just fill in a NaN
            image->data()[k]=dnan; 
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::clear()
{
    FloatImg* image =  dynamic_cast<FloatImg*>(m_image); 
    size_t s = image->data().size();
    for( size_t k = 0; k< s; ++k){
        // determine the bin center
        float 
            x = static_cast<int>(k%m_naxis1)+0.5, 
            y = static_cast<int>(k/m_naxis1)+0.5;
        try{
            astro::SkyDir dir(x,y,astro::SkyDir::PROJECTION);
            image->data()[k] = 0; 
        }catch(... ) { // any exception: just fill in a NaN
            image->data()[k]=dnan; 
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::~SkyImage()
{
    FloatImg* image =  dynamic_cast<FloatImg*>(m_image); 
    if(image!=0){
        image->saveElement();
        delete image;
    }
}
