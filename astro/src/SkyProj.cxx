/** @file SkyProj.cxx
@brief implementation of the class SkyProj

*/

// Include files

#include "astro/SkyProj.h"
#include "wcslib/wcs.h"

using namespace astro;
#include <string>
#include <string.h>
#include <stdexcept>



class SkyProj::Exception : public std::exception 
{
public:
   Exception() {}
   Exception(std::string errorString) 
      : m_what(errorString)
   {}

   virtual ~Exception() throw() {}
   virtual const char *what() const throw() {return m_what.c_str();}
protected:
   std::string m_what;
};





SkyProj::SkyProj(const std::string &projName, 
                 double crpix1,
                 double crpix2,
                 double crval1,
                 double crval2,
                 double cdelt1,
                 double cdelt2,
                 double crota1,
                 double crota2
                 )
{
   int i, j, status;
   double *pcij;
   char CTYPE[2][9] = {"XLAT-xxx", "XLON-xxx"};

   double pc[2][2] = {{1,0},
                      {0,1}};
   int naxis = 2;

   strncpy(&CTYPE[0][5], projName.c_str(), 3);
   strncpy(&CTYPE[1][5], projName.c_str(), 3);

   wcs = new wcsprm;
   wcs->flag = -1;
   wcsini(1, naxis, wcs);

   wcs->crpix[0] = crpix1;
   wcs->crpix[1] = crpix2;

   pcij = wcs->pc;
   for (i = 0; i < naxis; i++) {
      for (j = 0; j < naxis; j++) {
         *(pcij++) = pc[i][j];
      }
   }

   wcs->cdelt[0] = cdelt1;
   wcs->cdelt[1] = cdelt2;


   strcpy(wcs->ctype[0], CTYPE[0]);
   strcpy(wcs->ctype[1], CTYPE[1]);


   wcs->crval[0] = crval1;
   wcs->crval[1] = crval2;

   // Default's specified in twcs1.c test program
   // May need to be changed.
   wcs->lonpole = 150.0;
   wcs->latpole = 999.0;

   // Setting the following to a default value.
   // Might be unneccessary
   wcs->restfrq = 1.42040575e9f;
   wcs->restwav = 0.0f;

   // Set wcs to use CROTA rotations instead of PC or CD 
   // transformations
   wcs->altlin &= 4;

   wcs->crota[0] = crota1;
   wcs->crota[1] = crota2;

   // No PV cards
   wcs->npv = 0;

   /* Extract information from the FITS header. */
   if (status = wcsset2(wcs)) {
      printf("wcsset error%3d\n", status);
   }

}

SkyProj::~SkyProj()
{
   wcsfree(wcs);
   delete wcs;
}


/** @brief Do the projection with the given coordinates
@param s1 ra or l, in degrees
@param s2 dec or b, in degrees
*/
std::pair<double,double> SkyProj::project(double s1, double s2)
{
   int ncoords = 1;
   int nelem = 9;
   double worldcrd[9], imgcrd[9], pixcrd[9];
   double phi[1], theta[1];
   int stat[1];

   // WCS projection routines require the input coordinates are in degrees
   // and in the range of [-90,90] for the lat and [-180,180] for the lon.
   // So correct for this effect.
   if(s1 > 180) s1 -= 360.;

   worldcrd[wcs->lng] = s1;
   worldcrd[wcs->lat] = s2;

   wcss2p(wcs, ncoords, nelem, worldcrd, phi, theta, imgcrd, pixcrd, stat);

   return std::make_pair<double,double>(pixcrd[wcs->lng],pixcrd[wcs->lat]);
}

std::pair<double,double> SkyProj::deproject(double x1, double x2)
{
   int ncoords = 1;
   int nelem = 9;
   double s1;
   double worldcrd[9], imgcrd[9], pixcrd[9];
   double phi[1], theta[1];
   int stat[1];

   pixcrd[wcs->lng] = x1;
   pixcrd[wcs->lat] = x2;

   wcsp2s(wcs, ncoords, nelem, pixcrd, imgcrd, phi, theta, worldcrd, stat);

   s1 = worldcrd[wcs->lng];

   //fold RA into the range (0,360)
   while(s1 < 0) s1 +=360.;
   while(s1 > 360) s1 -= 360.;

   return std::make_pair<double,double>(s1,worldcrd[wcs->lat]);
}


/** @brief Convert from one projection to another
@param x1 projected equivalent to ra or l, in degrees
@param x2 projected equivalent dec or b, in degrees
@param projection used to deproject these coordinates
*/
std::pair<double,double> SkyProj::project(double x1, double x2, SkyProj otherProjection)
{
   double s1, s2; // Sky Coordinates
   std::pair<double,double> s;
   s = otherProjection.deproject(x1,x2);
   s1 = s.first;
   s2 = s.second;
   return SkyProj::project(s1,s2);
}



