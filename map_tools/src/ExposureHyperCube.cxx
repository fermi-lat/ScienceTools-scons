/**
* @file ExposureHyperCube.cxx
@brief Implement ExposureHyperCube methods
*
* $Header$
*/
#include "map_tools/ExposureHyperCube.h"

#include "tip/Image.h"
#include "tip/IFileSvc.h"

using namespace map_tools;
namespace {
    //! @brief add a string or double key or whatever to the image 
    tip::Header* header;
    template <typename T>
        void setKey(std::string name, T value, std::string /*unit*/="", std::string /*comment*/=""){
            (*header)[name].set( value); }
}
ExposureHyperCube::ExposureHyperCube( const Exposure& exp, 
                                     std::string outfile, bool clobber) : m_image(0)
{
    std::vector<long> naxes(3);
    naxes[0]= Exposure::Index::ra_factor;
    naxes[1]= Exposure::Index::dec_factor;
    naxes[2] =Exposure::Index::cosfactor;
 
    if(clobber ){
        // the new way to rewrite a file
        tip::IFileSvc::instance().createFile(outfile);
    }
 
    tip::IFileSvc::instance().createImage(outfile, "exposure", naxes);
    m_image = tip::IFileSvc::instance().editImage(outfile, "exposure");
    header = &m_image->getHeader();// set up the anonymous convenience functions


    setKey("TELESCOP", "GLAST");
    setKey("INSTRUME", "LAT");

    setKey("DATE-OBS", "");
    setKey("DATE-END", "");
    setKey("EQUINOX", 2000.0);

    setKey("CTYPE1", "RA");
    setKey("CRPIX1",  0.5*(1+naxes[0]));
    setKey("CRVAL1",  0);
    setKey("CDELT1",  -1.);
    setKey("CUNIT1",  "deg");

    setKey("CTYPE2",  "DEC");
    setKey("CRPIX2",  0.5*(1+naxes[1]));
    setKey("CRVAL2",  0.);
    setKey("CDELT2",  1.0);
    setKey("CUNIT2",  "deg");

    setKey("CTYPE3",  Exposure::Index::thetaBinning()); //"SQRT(1-COSTHETA)");
    setKey("CRPIX3",   0.5*(1+naxes[2]) );
    setKey("CRVAL3",  0.);
    setKey("CDELT3",  1./naxes[2] );
    setKey("CUNIT3",  "");

    setKey("BUNIT", "s");

    setKey("CROTA2",  0);
    // record total exposure as a key
    setKey("TOTAL", exp.total());

    m_image->set( exp.data() );

}
void ExposureHyperCube::save()
{
    delete m_image;
    m_image=0;
}
ExposureHyperCube::~ExposureHyperCube()
{
    delete m_image;
}
