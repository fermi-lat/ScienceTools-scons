/** @file SkyImage.cxx

@brief implement the class SkyImage
$Header$
*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "astro/SkyDir.h"
#include "astro/SkyFunction.h"
#include "tip/Image.h"
#include "tip/IFileSvc.h"
#include <stdexcept>
#include <sstream>

namespace {
    static unsigned long lnan[2]={0xffffffff, 0x7fffffff};
    static double& dnan = *( double* )lnan;
    //! @brief add a string or double key or whatever to the image 
    tip::Header* header;
    template <typename T>
    void setKey(std::string name, T value, std::string /*unit*/="", std::string /*comment*/=""){
        (*header)[name].set( value); }
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
, m_wcs(0)
{
    using namespace astro;
    std::string ptype(pars.projType());
    std::string extension("skyimage"); // maybe a parameter?
    double pixelsize = pars["pixelsize"];

    if( m_naxis1==0){
        // special code to determine all-sky limits based on scale factor and transformation
        std::string types[]={"" ,"CAR","AIT","ZEA"};
        int xsize[] =       {360, 360,  325,  230}; 
        int ysize[] =       {180, 180,  163,  230}; 
        for( unsigned int i = 0; i< sizeof(types)/sizeof(std::string); ++i){
            if( ptype == types[i]) {
                m_naxis1 = static_cast<int>(xsize[i]/pixelsize);
                m_naxis2 = static_cast<int>(ysize[i]/pixelsize);
                break;
            }
        }
        if( m_naxis1==0) {
            throw std::invalid_argument("SkyImage::SkyImage -- projection type " 
                +ptype +" does not have default image size");
        }
    }
    bool galactic = pars.uselb();

    /// arrays describing transformation: assume reference in the center
    double          //lon            lat
        crval[2]={ pars.xref(),      pars.yref()},
        crpix[2]={ (m_naxis1+1)/2.0, (m_naxis2+1)/2.0},
        cdelt[2]={ -pixelsize,       pixelsize },
        crota2=pars.rot();
    m_wcs = new astro::SkyProj( pars.projType(), crpix, crval, cdelt, crota2, galactic);

    // setup the image: it needs an axis dimension array and the file name to write to
    std::vector<long> naxes(3);
    naxes[0]=m_naxis1;
    naxes[1]=m_naxis2;
    naxes[2]=m_naxis3;

#if 0 // failed: needs a minimal template
    if( pars.clobber() ){
        // the new way to rewrite a file
        tip::IFileSvc::instance().createFile(pars.outputFile());
    }
#endif
    // now add an image to the file
    tip::IFileSvc::instance().createImage(pars.outputFile(), extension, naxes);
    m_image = tip::IFileSvc::instance().editImage(pars.outputFile(), extension);

    m_pixelCount = m_naxis1*m_naxis2*m_naxis3;
    m_imageData.resize(m_pixelCount);

    // fill the boundaries with NaN
   //THB, until know why it takes time if( pars.projType()!="CAR") clear();

    header = &m_image->getHeader();// set up the anonymous convenience functions

    setKey("TELESCOP", "GLAST");

    setKey("INSTRUME", "LAT SIMULATION");

    setKey("DATE-OBS", "");
    setKey("DATE-END", "");
    setKey("EQUINOX", 2000.0,"","Equinox of RA & DEC specifications");

    setKey("CTYPE1", std::string(galactic?"GLON-":"RA---")+ pars.projType()
        ,"","[RA|GLON]---%%%, %%% represents the projection method such as AIT");
    setKey("CRPIX1",  crpix[0],"","Reference pixel"); 
    setKey("CRVAL1",  crval[0], "deg", "RA or GLON at the reference pixel");
    setKey("CDELT1",  cdelt[0],"",
        "X-axis incr per pixel of physical coord at position of ref pixel(deg)");

    setKey("CTYPE2",  std::string(galactic?"GLAT-":"DEC--")+ pars.projType()
        ,"","[DEC|GLAT]---%%%, %%% represents the projection method such as AIT");

    setKey("CRPIX2",  crpix[1],"","Reference pixel");
    setKey("CRVAL2",  crval[1], "deg", "DEC or GLAT at the reference pixel"); 
    setKey("CDELT2",  cdelt[1],"",
        "Y-axis incr per pixel of physical coord at position of ref pixel(deg)");

    setKey("CROTA2",  crota2, "", "Image rotation (deg)");
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::SkyImage(const std::string& fits_file, const std::string& extension)
:  m_save(false)
, m_layer(0)
, m_wcs(0)
{
    m_image = const_cast<tip::Image*>(tip::IFileSvc::instance().readImage(fits_file, extension));
    tip::Header& header = m_image->getHeader();

    // standard ordering for ra, dec, cos(theta).
    header["NAXIS1"].get(m_naxis1);
    header["NAXIS2"].get(m_naxis2);
    header["NAXIS3"].get(m_naxis3);
    m_pixelCount = m_naxis1*m_naxis2*m_naxis3;

    std::string ctype;
    header["CTYPE1"].get(ctype);
    bool galactic;
    if( ctype.substr(0,2)=="RA") {
        galactic=false;
    }else if( ctype.substr(0,4)=="GLON") {
        galactic=true;
    }else {
        throw std::invalid_argument(
            std::string("SkyImage::SkyImage -- unexpected CYTPE1 value: ")+ctype);
    }

    // note that the ctype may be blank: wcslib treats this like CAR
    std::string  trans(ctype.size()==8?ctype.substr(5,3): "");

    /// arrays describing transformation; pointers passed to wcslib
    double crpix[2], crval[2], cdelt[2];

    header["CRPIX1"].get( crpix[0]);
    header["CRVAL1"].get( crval[0]);
    header["CDELT1"].get( cdelt[0]);

    header["CRPIX2"].get( crpix[1]);
    header["CRVAL2"].get( crval[1]);
    header["CDELT2"].get( cdelt[1]);
    double crota2=0;
    try { header["CROTA2"].get(crota2);}catch(const std::exception&){}
    m_wcs = new astro::SkyProj(trans, crpix, crval, cdelt, crota2, galactic);

    // finally, read in the image
    m_image->get(m_imageData);

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unsigned int SkyImage::setLayer(unsigned int newlayer)
{
    checkLayer(newlayer);
    unsigned int t = m_layer;
    m_layer = newlayer;
    return t;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::addPoint(const astro::SkyDir& dir, double delta, unsigned int layer)
{
    std::pair<double,double> p= dir.project(*m_wcs);
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if(  k< m_pixelCount){
        m_imageData[k] += delta;
        m_total += delta;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::checkLayer(unsigned int layer)const
{
    if( layer >= (unsigned int)m_naxis3){
        std::stringstream errmsg;
        errmsg << "SkyImage: requested layer " << layer 
            << " not compatible with axis3: " << m_naxis3 << std::endl;
        throw std::out_of_range(errmsg.str());
    }

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::fill(const astro::SkyFunction& req, unsigned int layer)
{
    checkLayer(layer);
    int offset = m_naxis1* m_naxis2 * layer;

    for( size_t k = 0; k< (unsigned int)(m_naxis1* m_naxis2); ++k){
        // determine the bin center (pixel coords start at (1,1) in center of lower left
        double 
            x = static_cast<int>(k%m_naxis1)+1.0, 
            y = static_cast<int>(k/m_naxis1)+1.0;
        try{

            astro::SkyDir dir(x,y, *m_wcs);
            double t= req(dir);
            m_imageData[k+offset] = t;
            m_total += t;
        }catch(... ) { // any exception: just fill in a NaN
            m_imageData[k+offset]=dnan; 
        }
    }

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::clear()
{
    size_t s = m_imageData.size();
    size_t pixels = size_t(m_naxis1*m_naxis2);
    for( size_t k = 0; k< s; ++k){
        // determine the bin center
        size_t kk = k%pixels;

        double
            x = static_cast<int>(kk%m_naxis1)+1, 
            y = static_cast<int>(kk/m_naxis1)+1;
        try{
            astro::SkyDir dir(x,y, *m_wcs);
            m_imageData[k] = 0; 
        }catch(... ) { // any exception: just fill in a NaN
            m_imageData[k]=dnan; 
        }
    }

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::~SkyImage()
{
    if( m_save) {
        m_image->set(m_imageData);
    }
    delete m_image; 
    delete m_wcs;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double SkyImage::pixelValue(const astro::SkyDir& pos,unsigned  int layer)const
{
    checkLayer(layer); 
    unsigned int k = pixel_index(pos,layer);
    return m_imageData[k];        
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
float &  SkyImage::operator[](const astro::SkyDir&  pixel)
{
    unsigned int k = pixel_index(pixel);
    return m_imageData[k];        

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const float &  SkyImage::operator[](const astro::SkyDir&  pixel)const
{
    unsigned int k = pixel_index(pixel);
    return m_imageData[k];        

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double SkyImage::operator()(const astro::SkyDir& s)const
{
    return pixelValue(s, m_layer);        
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::getNeighbors(const astro::SkyDir& pos, std::vector<double>&neighbors)const
{
    int layer = 0; ///@todo: get neighbors on a different layer
    std::pair<double,double> p= pos.project(*m_wcs);
    if( p.first<0) p.first += m_naxis1;
    if(p.second<0) p.second += m_naxis2;
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if(i+1<(unsigned int)m_naxis1)neighbors.push_back(m_imageData[k+1]); 
    if(i>0) neighbors.push_back(m_imageData[k-1]);
    if(j+1<(unsigned int)m_naxis2)neighbors.push_back(m_imageData[k+m_naxis1]);
    if(j>0)neighbors.push_back(m_imageData[k-m_naxis1]);

}

// internal routine to convert a SkyDir to a pixel index
unsigned int SkyImage::pixel_index(const astro::SkyDir& pos, int layer) const
{
    // if not specified, use the data member
    if( layer<0 ) layer = m_layer;

    // project using wcslib interface, then adjust to be positive
    std::pair<double,double> p= pos.project(*m_wcs);
    if( p.first<0) p.first += m_naxis1;
    if(p.second<0) p.second += m_naxis2;
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if( k >= m_pixelCount ) {
        throw std::range_error("SkyImage::operator[]-- outside image hyper cube");
    }
    return k;
}


