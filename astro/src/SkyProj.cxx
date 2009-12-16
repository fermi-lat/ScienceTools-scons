/** @file SkyProj.cxx
@brief implementation of the class SkyProj

$Header$
*/

// Include files

#include <cstring>
#include "tip/IFileSvc.h"
#include "tip/Image.h"
#include "tip/Header.h"
#include "astro/SkyProj.h"
#include "wcslib/wcs.h"
#include "wcslib/wcshdr.h"

//#include "longnam.h"
#ifndef WCSLIB_GETWCSTAB
#define WCSLIB_GETWCSTAB
#endif
#include "fitsio.h"

using namespace astro;
#include <string>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <cassert>
#include <iostream>

namespace{
/** @class SkyProjException
    @brief local std::exception to implement a what method with the status code
*/
class SkyProjException : public std::exception 
{
public:
    SkyProjException(int status=0) 
        : m_status(status)
    {}

    virtual ~SkyProjException() throw() {}
    virtual const char *what() const throw(){
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
}
SkyProj::SkyProj(const std::string &projName, 
                 double* crpix, double* crval, double* cdelt, double crota2 ,bool galactic)                
{
        tol=0.00000001;
	double lonpole = 999;
	double latpole = 999;
	SkyProj::init(projName,crpix,crval,cdelt,lonpole,latpole,crota2,galactic);
}

SkyProj::SkyProj(const std::string &projName, 
        double* crpix, double* crval, double* cdelt, double lonpole, double latpole,
		double crota2, bool galactic)
{
        tol=0.00000001;
        SkyProj::init(projName,crpix,crval,cdelt,lonpole,latpole,crota2,galactic);
}

SkyProj::SkyProj(const std::string & fitsFile, const std::string & extension) {
   const tip::Image * image = 
      tip::IFileSvc::instance().readImage(fitsFile, extension);

   const tip::Header & header = image->getHeader();

   bool galactic;
   std::string ctype;
   header["CTYPE1"].get(ctype);
   if (ctype.substr(0, 2) == "RA") {
      galactic = false;
   } else if (ctype.substr(0, 4) == "GLON") {
      galactic = true;
   } else {
      throw std::runtime_error("Unrecognized coordinate system in " 
                               + fitsFile + "[" + extension + "]");
   }
   
   std::string projName("");
   if (ctype.size() > 7) {
      projName = ctype.substr(ctype.size() - 3, 3);
   }
   
   double crpix[2], crval[2], cdelt[2];
   header["CRPIX1"].get(crpix[0]);
   header["CRVAL1"].get(crval[0]);
   header["CDELT1"].get(cdelt[0]);

   header["CRPIX2"].get(crpix[1]);
   header["CRVAL2"].get(crval[1]);
   header["CDELT2"].get(cdelt[1]);

   double lonpole;
   double latpole;
   try {
      header["LONPOLE"].get(lonpole);
      header["LATPOLE"].get(latpole);
   } catch (tip::TipException &) {
      lonpole = 999;
      latpole = 999;
   }

   double crota2(0);
   try {
      header["CROTA2"].get(crota2);
   } catch (tip::TipException &) {
   }

   delete image;

   init(projName, crpix, crval, cdelt, lonpole, latpole, crota2, galactic);
}

SkyProj::SkyProj(const std::string &fitsFile, int relax, int ctrl)
{
   int nreject;

   fitsfile *fptr; // cfitsio fits file pointer
   int mode = 0; // file read mode
   int fstatus = 0, // cfitsio error status 
       numkeys, // number of keys (cards) in header
       nummore; // number of keys that can be added to header
   char *header(0); // string containing header

   fits_open_file(&fptr,fitsFile.c_str(),mode,&fstatus);
   fits_report_error(stderr, fstatus);

   // Todo:  When the external library cfitsio is updated to 2.510 or later
   // replace ffghsp and fits_header2str with fits_hdr2str.

   // Get number of keywords in header
   ffghsp(fptr, &numkeys, &nummore, &fstatus);
   fits_report_error(stderr, fstatus);

   // Read header to string
   ffh2st(fptr, &header, &fstatus);
   fits_report_error(stderr, fstatus);

   fits_close_file(fptr,&fstatus);

   // wcspih reads the header string from the fits file and allocates 
   // memory for a wcsprm struct.
   wcspih(header,numkeys,relax,ctrl,&nreject,&m_nwcs,&m_wcs);
   m_wcspih_used = true;

   // Manually set naxis to 2. (three places?)
   m_wcs->naxis = m_wcs->lin.m_naxis = m_wcs->m_naxis = 2;

   int status = wcsset2(m_wcs);
   if (status !=0) {
       throw SkyProjException(status );
   }
   // and again, in case 
   m_wcs->naxis = m_wcs->lin.m_naxis = m_wcs->m_naxis = 2;

  // wcsprt(&m_wcs[0]); 
}

SkyProj::~SkyProj()
{
   if(m_wcspih_used)
      wcsvfree(&m_nwcs,&m_wcs);
   else
      wcsfree(m_wcs);
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
    if ( returncode != 0 ) throw SkyProjException(returncode);

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
    if ( returncode != 0 ) throw SkyProjException(returncode);

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
std::pair<double,double> SkyProj::pix2pix(double x1, double x2, const SkyProj& otherProjection)const
{
    std::pair<double,double> s = otherProjection.pix2sph(x1,x2);
    return SkyProj::sph2pix(s.first,s.second);
}

bool SkyProj::isGalactic()const
{
    return ( std::string( m_wcs->ctype[0] ).substr(0,4)=="GLON");
}

/*@brief determine the x or y range for a given x or y coordinate
   @param xvar varies x if true, varies y if false
   @param x1 x or y coordinate to find y or x range respectively */
std::pair<double,double> SkyProj::range(double x1,bool xvar) {
    if(hasRange(x1,xvar))
        return std::make_pair<double,double>(0,0);
    double xval,yval,delt;
    double max,min;
    int offset;
    if(xvar) {
        //y is constant
        xval=limit[0];
        yval=x1;
        delt=(xval-limit[1])/4;
        offset=0;
    }
    else {
        //x is contant
        xval=x1;
        yval=limit[2];
        delt=(yval-limit[3])/2;
        offset=2;
    }
    int returncode = -1;
    //find max by a binary search
    while(delt/limit[offset] > tol || returncode!=0) {
        returncode = testpix2sph(xval,yval);
        if(returncode==0&&xvar) 
            xval=xval+delt;
        else if(returncode==0&&!xvar) 
            yval=yval+delt;
        else if(returncode!=0&&xvar)
            xval=xval-delt;
        else
            yval=yval-delt;
        delt=delt/2;
    }
    if(xvar) {
        max=xval;
        xval=limit[1];
        yval=x1;
        delt=(limit[0]-xval)/4;
        offset=0;
    }
    else {
        max=yval;
        xval=x1;
        yval=limit[3];
        delt=(limit[2]-yval)/4;
        offset=2;
    }
    returncode=-1;
    //find min with a binary search
    while(delt/limit[offset+1] > tol || returncode!=0) {
        returncode = testpix2sph(xval,yval);
        if(returncode==0&&xvar) 
            xval=xval-delt;
        else if(returncode==0&&!xvar) 
            yval=yval-delt;
        else if(returncode!=0&&xvar)
            xval=xval+delt;
        else
            yval=yval+delt;
        delt=delt/2;
    }
    xvar?min=xval:min=yval;
    return std::make_pair<double,double>(max,min);
}

/*@brief determines if a pixel coordinate is in 
    the scope of the projection*/
int SkyProj::testpix2sph(double x1, double x2) const{
    int ncoords = 1;
    int nelem = 2;
    double worldcrd[2], imgcrd[2];
    double phi[1], theta[1];
    int stat[1];

    double pixcrd[] = {x1,x2};;

    try{
        m_wcs->lin.naxis=2; // don't know why this is needed
    return wcsp2s(m_wcs, ncoords, nelem, pixcrd, imgcrd, phi, theta, worldcrd, stat);
    }catch(...){
        std::cout << "testpix2ph: unexpected failure for " << x1 <<", "<< x2 << std::endl; 
        return 1;
    }
}

void SkyProj::init(const std::string &projName, 
                 double* crpix, double* crval, double* cdelt, 
				 double lonpole, double latpole, double crota2, bool galactic)
{
    assert( sizeof_wcslib>=sizeof(wcsprm));
    m_wcs = reinterpret_cast<wcsprm*>(m_wcs_struct);
    m_wcs->flag = -1;

    m_projName = projName; // save for user access

    m_wcspih_used = false;

    int naxis = 2;
    wcsini(1, naxis, m_wcs);

    std::string 
        lon_type = (galactic? "GLON" : "RA"),
        lat_type =  (galactic? "GLAT" : "DEC");

    if(projName.compare("") != 0) {
       lon_type += (galactic? "-" : "---") + projName;
       lat_type += (galactic? "-" : "--") + projName;
    }

    strcpy(m_wcs->ctype[0], lon_type.c_str() );
    strcpy(m_wcs->ctype[1], lat_type.c_str() );

    // copy  intput arrays
    for( int i=0; i<naxis; ++i){
        m_wcs->crval[i] = crval[i];  // reference value
        m_wcs->crpix[i] = crpix[i]; // pixel coordinate
        m_wcs->cdelt[i] = cdelt[i]; // scale factor
    }

	if(latpole != 999)
		m_wcs->latpole = latpole;
	if(lonpole != 999)
		m_wcs->lonpole = lonpole;


    // Set wcs to use CROTA rotations instead of PC or CD  transformations
    m_wcs->altlin |= 4;
    m_wcs->crota[1] = crota2;
    
    int status = wcsset2(m_wcs);
    if (status !=0) {
        throw SkyProjException(status );
    }

    // enable this to see a nice formatted dump
    //wcsprt(m_wcs);

    //determine bounding box
    try {
        findBound(crpix);
    }catch(...){
        std::cerr << "warning: could not find bounding box" << std::endl;
    }

    // a simple test
    double tlon = crval[0], tlat = crval[1];
    std::pair<double, double> t = sph2pix(tlon, tlat);
    double check = fabs(t.first-crpix[0])+fabs(t.second-crpix[1]);
    std::pair<double, double> s = pix2sph(t.first, t.second);
    check = fabs(s.first-crval[0]-s.second-crval[1]);

}

SkyProj::SkyProj(const SkyProj & other)
{
    // copy constructor just resets pointer
    assert( sizeof_wcslib>=sizeof(wcsprm));
    m_wcs = reinterpret_cast<wcsprm*>(m_wcs_struct);    
}

/** @brief determines if current point and variable are in range */
bool SkyProj::hasRange(double x1, bool xvar) {
    return (xvar&&limit[4]==1)||(!xvar&&limit[5]==1)||((x1>limit[2]||x1<limit[3])&&xvar)||((x1>limit[0]||x1<limit[1])&&!xvar);
}

/** @brief initializes the bounding rectangle
        breaks if rectangle has no width or height */
void SkyProj::findBound(double* crpix) {
    int returncode = 0;
    double j=crpix[0];
    returncode = testpix2sph(0,max+crpix[1]); //tests max x finite
    returncode==0?limit[4]=1:limit[4]=0;
    returncode = testpix2sph(max+crpix[0],0); //tests max y finite
    returncode==0?limit[5]=1:limit[5]=0;
    double delt = max/4;
    double x1 = max/2+crpix[0];
    int k=0;
    //try a binary search in all directions for limits
    //test xmax
    while((delt>tol || returncode!=0) && limit[4]==0) {
        returncode = testpix2sph(x1,crpix[1]);
        if(returncode!=0)
            x1=x1-delt;
        else
            x1=x1+delt;
        delt=delt/2;
        if(k>600)
            throw std::runtime_error("This transformation has no valid x range.");
        k++;
    }
    limit[0]=x1;
    delt = -max/4;
    //test xmin
    x1 = -max/2+crpix[0];
    while((-delt>tol || returncode!=0) && limit[4]==0) {
        returncode = testpix2sph(x1,crpix[1]);
        if(returncode!=0)
            x1=x1-delt;
        else
            x1=x1+delt;
        delt=delt/2;
    }
    limit[1]=x1;
    delt = max/4;
    //test ymax
    x1 = max/2+crpix[1];
    k=0;
    while((delt>tol || returncode!=0) && limit[4]==0) {
        returncode = testpix2sph(crpix[0],x1);
        if(returncode!=0)
            x1=x1-delt;
        else
            x1=x1+delt;
        delt=delt/2;
        if(k>600)
            throw std::runtime_error("This transformation has no valid y range.");
        k++;
    }
    limit[2]=x1;
    delt = -max/4;
    //test ymin
    x1 = -max/2+crpix[1];
    while((-delt>tol || returncode!=0) && limit[4]==0 && k<600) {
        returncode = testpix2sph(crpix[0],x1);
        if(returncode!=0)
            x1=x1-delt;
        else
            x1=x1+delt;
        delt=delt/2;
    }
    limit[3]=x1;
}

namespace {
        tip::Header* header;

template <typename T>
        void setKey(std::string name, T value, std::string unit="", std::string comment=""){
            (*header)[name].set( value); 
            (*header)[name].setUnit(unit);
            (*header)[name].setComment(comment);
        }
}
void SkyProj::setKeywords(tip::Header& hdr)
{
    header = &hdr;
    setKey("TELESCOP", "GLAST");

    setKey("INSTRUME", "LAT");

    setKey("DATE-OBS", "");
    setKey("DATE-END", "");
    setKey("EQUINOX", 2000.0,"","Equinox of RA & DEC specifications");

    setKey("CTYPE1", m_wcs->ctype[0]
        ,"","[RA|GLON]---%%%, %%% represents the projection method such as AIT");
    setKey("CRPIX1",  m_wcs->crpix[0],"","Reference pixel"); 
    setKey("CRVAL1",  m_wcs->crval[0], "deg", "RA or GLON at the reference pixel");
    setKey("CDELT1",  m_wcs->cdelt[0],"",
        "X-axis incr per pixel of physical coord at position of ref pixel(deg)");

    setKey("CTYPE2",  m_wcs->ctype[1]
        ,"","[DEC|GLAT]---%%%, %%% represents the projection method such as AIT");

    setKey("CRPIX2",  m_wcs->crpix[1],"","Reference pixel");
    setKey("CRVAL2",  m_wcs->crval[1], "deg", "DEC or GLAT at the reference pixel"); 
    setKey("CDELT2",  m_wcs->cdelt[1],"",
        "Y-axis incr per pixel of physical coord at position of ref pixel(deg)");

    // todo: fix these
    setKey("CROTA2",  0, "", "Image rotation (deg)");
    if( std::string(m_wcs->ctype[0]).substr(5,3)=="CAR") return;
    // these may be needed by ds9 for non-CAR. I get confused
    setKey("LONPOLE", 180.0, "deg", "longitude of celestial pole");
    setKey("LATPOLE", 0,     "deg", "latitude of celestial pole");
}

