/** @file SkyProj.cxx
@brief implementation of the class SkyProj
$Header$
*/

// Include files

#include "astro/SkyProj.h"
#include "wcslib/wcs.h"

using namespace astro;
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>

class SkyProj::Exception : public std::exception 
{
public:
    Exception() {}
    Exception(int status) 
        : m_status(status)
    {}

    virtual ~Exception() throw() {}
    virtual const char *what() const throw() {
        std::stringstream msg; 
        msg << "SkyProj wcslib error "<< m_status << " : ";
        if(  m_status<1 || m_status>11 ) msg << " unknown error";
        else msg << wcs_errmsg[m_status];
        static char  buf[80];
        ::strncpy(buf, msg.str().c_str(), sizeof(buf));
        return buf;
    }
    int status()const throw(){return m_status;}
private:
    int m_status;
};

SkyProj::SkyProj(const std::string &projName, 
                 double* crpix, double* crval, double* cdelt, double crota2 ,bool galactic )
{

    m_wcs = reinterpret_cast<wcsprm*>(new char[sizeof(wcsprm)]);
    m_wcs->flag = -1;

    int naxis = 2;
    wcsini(1, naxis, m_wcs);

    std::string 
        lon_type = (galactic? "GLON-" : "RA---") + projName,
        lat_type =  (galactic? "GLAT-" : "DEC--") + projName;
    strcpy(m_wcs->ctype[0], lon_type.c_str() );
    strcpy(m_wcs->ctype[1], lat_type.c_str() );

    // copy  intput arrays
    for( int i=0; i<naxis; ++i){
        m_wcs->crval[i] = crval[i];  // reference value
        m_wcs->crpix[i] = crpix[i]; // pixel coordinate
        m_wcs->cdelt[i] = cdelt[i]; // scale factor
    }

    // specify position of pole
#if 0 // not now
    m_wcs->lonpole = crval[0];
    m_wcs->latpole=crval[1];
#endif
    // Set wcs to use CROTA rotations instead of PC or CD  transformations
    m_wcs->altlin |= 4;
    m_wcs->crota[1] = crota2;

    int status = wcsset2(m_wcs);
    if (status !=0) {
        throw SkyProj::Exception(status );
    }
    // a simple test
    double tlon = crval[0], tlat = crval[1];
    std::pair<double, double> t = sph2pix(tlon, tlat);
    double check = fabs(t.first-crpix[0])+fabs(t.second-crpix[1]);
    std::pair<double, double> s = pix2sph(t.first, t.second);
    check = fabs(s.first-crval[0]-s.second-crval[1]);

    wcsprt(m_wcs);// temp
}

SkyProj::~SkyProj()
{
    wcsfree(m_wcs);
    delete m_wcs;
}


/** @brief Do the projection to pixels with the given coordinates
@param s1 ra or l, in degrees
@param s2 dec or b, in degrees
@return pair(x,y) in pixel coordinates
*/
std::pair<double,double> SkyProj::sph2pix(double s1, double s2) const
{
    int ncoords = 1;
    int nelem = 2;
    double  imgcrd[2], pixcrd[2];
    double phi[1], theta[1];
    int stat[1];

    // WCS projection routines require the input coordinates are in degrees
    // and in the range of [-90,90] for the lat and [-180,180] for the lon.
    // So correct for this effect.
    if(s1 > 180) s1 -= 360.;

    double worldcrd[] ={s1,s2};

    int returncode = wcss2p(m_wcs, ncoords, nelem, worldcrd, phi, theta, imgcrd, pixcrd, stat);
    if ( returncode != 0 ) throw SkyProj::Exception(returncode);

    return std::make_pair(pixcrd[0],pixcrd[1]);
}

std::pair<double,double> SkyProj::pix2sph(double x1, double x2) const
{
    int ncoords = 1;
    int nelem = 2;
    double worldcrd[2], imgcrd[2];
    double phi[1], theta[1];
    int stat[1];

    double pixcrd[] = {x1,x2};;

    int returncode = wcsp2s(m_wcs, ncoords, nelem, pixcrd, imgcrd, phi, theta, worldcrd, stat);
    if ( returncode != 0 ) throw SkyProj::Exception(returncode);

    double s1 = worldcrd[0];

    //fold RA into the range [0,360)
    while(s1 < 0) s1 +=360.;
    while(s1 >= 360) s1 -= 360.;

    return std::make_pair<double,double>(s1,worldcrd[1]);
}


/** @brief Convert from one projection to another
@param x1 projected equivalent to ra or l, in degrees
@param x2 projected equivalent dec or b, in degrees
@param projection used to deproject these coordinates
*/
std::pair<double,double> SkyProj::pix2pix(double x1, double x2, SkyProj otherProjection)
{
    std::pair<double,double> s = otherProjection.pix2sph(x1,x2);
    return SkyProj::sph2pix(s.first,s.second);
}

bool SkyProj::isGalactic()const
{
    return ( std::string( m_wcs->ctype[0] ).substr(0,4)=="GLON");
};
