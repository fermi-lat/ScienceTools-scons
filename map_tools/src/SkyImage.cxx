/** @file SkyImage.cxx

@brief implement the class SkyImage

*/

#include "map_tools/SkyImage.h"
#include "map_tools/MapParameters.h"

#include "astro/SkyDir.h"
namespace {
    static unsigned long lnan[2]={0xffffffff, 0x7fffffff};
    static double& dnan = *( double* )lnan;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
SkyImage::SkyImage(const MapParameters& pars)
: table::PrimaryHDU<float>() 
, m_naxis1(pars.npix())
, m_naxis2(pars.npixY())
, m_naxis3(1) // for future expansion
, m_total(0)
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
    std::vector<long> naxes(3);
    naxes[0]=m_naxis1;
    naxes[1]=m_naxis2;
    naxes[2]=m_naxis3;
    Primary::setNaxis(3);
    Primary::setNaxes(naxes);
    Primary::setBitpix( table::Ifloat);
    Primary::setBufferSize(m_naxis1*m_naxis2*m_naxis3);
    Primary::setScale(1.0);
    m_data.resize(m_naxis1*m_naxis2*m_naxis3);

    // fill the boundaries with NaN
#if 1// ?? takes too long, investigate
     if( pars.projType()=="AIT") clear();
#endif

    setKey("TELESCOP", "GLAST");
    setKey("INSTRUME", "SIMULATION");

    setKey("DATE-OBS", "");
    setKey("DATE-END", "");
    setKey("EQUINOX", 2000.0);

    setKey("CTYPE1", std::string(galactic?"GLON--":"RA--")+ pars.projType());
    setKey("CRPIX1",  SkyDir::s_refX+0.5); // note that FITS pixel reference is off by 0.5
    setKey("CRVAL1",  SkyDir::s_refRA);
    setKey("CDELT1",  SkyDir::s_scaleX);
    setKey("CUNIT1",  "deg");

    setKey("CTYPE2",  std::string(galactic?"GLAT--":"DEC--")+ pars.projType());
    setKey("CRPIX2",  SkyDir::s_refY+0.5);// note that FITS pixel reference is off by 0.5
    setKey("CRVAL2",  SkyDir::s_refDEC); 
    setKey("CDELT2",  SkyDir::s_scaleY);
    setKey("CUNIT2",  "deg");

    setKey("CROTA2",  0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::addPoint(const astro::SkyDir& dir, double delta, int layer){

    std::pair<double,double> p= dir.project();
    unsigned int 
        i = static_cast<unsigned int>(p.first),
        j = static_cast<unsigned int>(p.second),
        k = i+m_naxis1*(j + layer*m_naxis2);
    if(  k< m_data.size()){
        m_data[k]+=delta;
        m_total += delta;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::fill(Requester& req)
{

    for( size_t k = 0; k< m_data.size(); ++k){
        // determine the bin center
        float 
            x = static_cast<int>(k%m_naxis1)+0.5, 
            y = static_cast<int>(k/m_naxis1)+0.5;
        try{
            astro::SkyDir dir(x,y,astro::SkyDir::PROJECTION);
            double t= req(dir);
            m_data[k] = t; 
            m_total += t;
        }catch(... ) { // any exception: just fill in a NaN
            m_data[k]=dnan; 
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void SkyImage::clear()
{
    size_t s = m_data.size();
    for( size_t k = 0; k< s; ++k){
        // determine the bin center
        float 
            x = static_cast<int>(k%m_naxis1)+0.5, 
            y = static_cast<int>(k/m_naxis1)+0.5;
        try{
            astro::SkyDir dir(x,y,astro::SkyDir::PROJECTION);
            m_data[k] = 0; 
        }catch(... ) { // any exception: just fill in a NaN
            m_data[k]=dnan; 
        }
    }
}
