 /** @file SkyImage.cxx

@brief implement the class SkyImage

*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "astro/SkyDir.h"
#include "image/Image.h"
#include "image/IOElement.h"

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
, m_naxis3(pars.npixZ()) // for future expansion
, m_total(0)
, m_image(0)
, m_save(true)
, m_layer(0)
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
        ,"","[RA|GLON]---%%%, %%% represents the projection method such as AIT");
    setKey("CRPIX1",  SkyDir::s_refX+0.5,"","Reference pixel"); // note that FITS pixel reference is off by 0.5
    setKey("CRVAL1",  pars.xref(), "deg", "RA or GLON at the reference pixel");
    setKey("CDELT1",  SkyDir::s_scaleX,"",
        "X-axis incr per pixel of physical coord at position of ref pixel(deg)");
    setKey("CUNIT1",  "deg", "", "Physical unit of X-axis");

    setKey("CTYPE2",  std::string(galactic?"GLAT--":"DEC--")+ pars.projType()
        ,"","[DEC|GLAT]---%%%, %%% represents the projection method such as AIT");

    setKey("CRPIX2",  SkyDir::s_refY+0.5,"","Reference pixel");// note that FITS pixel reference is off by 0.5
    setKey("CRVAL2",  pars.yref(), "deg", "DEC or GLAT at the reference pixel"); 
    setKey("CDELT2",  SkyDir::s_scaleY,"",
        "Y-axis incr per pixel of physical coord at position of ref pixel(deg)");
    setKey("CUNIT2",  "deg", "", "Physical unit of Y-axis");

    setKey("CROTA2",  0, "", "Image rotation (deg)");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::SkyImage(const std::string& fits_file, const std::string& extension)
:  m_save(false)
{
    FloatImg& image = *dynamic_cast<FloatImg*>(IOElement::readIOElement(fits_file, extension));
    m_image = &image;

    // standard ordering for ra, dec, cos(theta).
    image.getValue("NAXIS1", ( m_naxis1));
    image.getValue("NAXIS2", (m_naxis2));
    image.getValue("NAXIS3", (m_naxis3));
    m_pixelCount = m_naxis1*m_naxis2*m_naxis3;

    std::string ctype, trans;
    bool uselb;
    image.getValue("CTYPE1", ctype);
    if( ctype.substr(0,2)=="RA") {
        uselb=false;
        trans = ctype.substr(4,3);
    }else if( ctype.substr(0,4)=="GLON") {
        uselb=true;
        trans = ctype.size()==4? "CAR" : ctype.substr(6,3);
    }else {
        throw std::invalid_argument(
            std::string("SkyImage::SkyImage -- unexpected CYTPE1 value: ")+ctype);
    }

    double cr1[3];
    image.getValue("CRPIX1", cr1[0]);
    image.getValue("CRVAL1", cr1[1]);
    image.getValue("CDELT1", cr1[2]);

    double cr2[3];
    image.getValue("CRPIX2", cr2[0]);
    image.getValue("CRVAL2", cr2[1]);
    image.getValue("CDELT2", cr2[2]);
#if 0 /// @todo: interpret layer info if present
    double cr3[3];
    image.getValue("CRPIX3", cr3[0]);
    image.getValue("CRVAL3", cr3[1]);
    image.getValue("CDELT3", cr3[2]);
#endif
    double crota2=0.;
    try { image.getValue("CROTA2", crota2);}catch(const std::exception&){}
    
    astro::SkyDir::setProjection(
        cr1[1]*M_PI/180, cr2[1]*M_PI/180, //! @todo verify units 
        trans, 
        cr1[0]-0.5,      cr2[0]-0.5, 
        cr1[2],          cr2[2], 
        crota2, uselb);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unsigned int SkyImage::setLayer(unsigned int newlayer)
{
    if( newlayer>=static_cast<unsigned int>( m_naxis3))
        throw std::invalid_argument("SkyImage::setLayer-- invalid layer number");
    unsigned int t = m_layer;
    m_layer = newlayer;
    return t;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::addPoint(const astro::SkyDir& dir, double delta, unsigned int layer)
{
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
void SkyImage::fill(const astro::SkyFunction& req, unsigned int layer)
{
    if( layer >= (unsigned int)m_naxis3) throw std::out_of_range("SkyImage::fill, layer out of range");
    FloatImg* image =  dynamic_cast<FloatImg*>(m_image); 
    int offset = m_naxis1* m_naxis2 * layer;
    for( size_t k = 0; k< (unsigned int)(m_naxis1* m_naxis2); ++k){
        // determine the bin center
        double 
            x = static_cast<int>(k%m_naxis1)+0.5, 
            y = static_cast<int>(k/m_naxis1)+0.5;
        try{
            astro::SkyDir dir(x,y,astro::SkyDir::PROJECTION);
            double t= req(dir);
            image->data()[k+offset] = t; 
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
    size_t pixels = size_t(m_naxis1*m_naxis2);
    for( size_t k = 0; k< s; ++k){
        // determine the bin center
        size_t kk = k%pixels;

        double
            x = static_cast<int>(kk%m_naxis1)+0.5, 
            y = static_cast<int>(kk/m_naxis1)+0.5;
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
    if( !m_save) { delete m_image; return;}
    FloatImg* image =  dynamic_cast<FloatImg*>(m_image); 
    if(image!=0){
        image->saveElement();
        delete image;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double SkyImage::pixelValue(const astro::SkyDir& pos,unsigned  int layer)const
{
    if( layer >= (unsigned int)m_naxis3) throw std::out_of_range("SkyImage::fill, layer out of range");
    std::pair<double,double> p= pos.project();
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);

    if(  k< m_pixelCount){
        return  reinterpret_cast<FloatImg*>(m_image)->data()[k];        
    }else{
        throw std::range_error("SkyImage::pixelValue-- outside image hyper cube");
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float &  SkyImage::operator[](const astro::SkyDir&  pixel)
 {
    std::pair<double,double> p= pixel.project();
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + m_layer*m_naxis2);

    if(  k< m_pixelCount){
        return  reinterpret_cast<FloatImg*>(m_image)->data()[k];        
    }else{
        throw std::range_error("SkyImage::operator[]-- outside image hyper cube");
    }
 }
 //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const float &  SkyImage::operator[](const astro::SkyDir&  pixel)const
{
    std::pair<double,double> p= pixel.project();
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + m_layer*m_naxis2);

    if(  k< m_pixelCount){
        return  reinterpret_cast<FloatImg*>(m_image)->data()[k];        
    }else{
        throw std::range_error("SkyImage::operator[]-- outside image hyper cube");
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::getNeighbors(const astro::SkyDir& pos, std::vector<double>&neighbors)const
{
    int layer = 0; ///@todo: get neighbors on a different layer
    FloatImg& image =  *dynamic_cast<FloatImg*>(m_image); 
    std::pair<double,double> p= pos.project();
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if(i+1<(unsigned int)m_naxis1)neighbors.push_back(image[k+1]); 
    if(i>0) neighbors.push_back(image[k-1]);
    if(j+1<(unsigned int)m_naxis2)neighbors.push_back(image[k+m_naxis1]);
    if(j>0)neighbors.push_back(image[k-m_naxis1]);
}
