/** @file SkyProj.cxx
@brief implementation of the class SkyProj

*/

// Include files

#include "astro/SkyProj.h"

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

/** @brief Default projection is Hammer-Aitoff
*/
SkyProj::SkyProj()
{
   prjini(&prj);
   std::string projName = "AIT";
   SkyProj::SetProjection(projName);
}

/** @brief Constructor specified by Projection Code
@param projName String containing three char code
            Valid codes are:
            AZP: zenithal/azimuthal perspective 
            SZP: slant zenithal perspective 
            TAN: gnomonic 
            STG: stereographic 
            SIN: orthographic/synthesis 
            ARC: zenithal/azimuthal equidistant 
            ZPN: zenithal/azimuthal polynomial 
            ZEA: zenithal/azimuthal equal area 
            AIR: Airy 
            CYP: cylindrical perspective
            CEA: cylindrical equal area 
            CAR: Plate carree 
            MER: Mercator 
            SFL: Sanson-Flamsteed 
            PAR: parabolic 
            MOL: Mollweide 
            AIT: Hammer-Aitoff 
            COP: conic perspective 
            COE: conic equal area
            COD: conic equidistant
            COO: conic orthomorphic
            BON: Bonne
            PCO: polyconic
            TSC: tangential spherical cube
            CSC: COBE quadrilateralized spherical cube
            QSC: quadrilateralized spherical cube
*/
SkyProj::SkyProj(const std::string &projName)
{
   prjini(&prj);
   SkyProj::SetProjection(projName);
}

/** @brief Do the projection with the given coordinates
@param s1 ra or l, in degrees
@param s2 dec or b, in degrees
*/
std::pair<double,double> SkyProj::Project(double s1, double s2)
{
   double xa1[1], xa2[1];  // Projection Coordinates
   double sa1[1], sa2[1];
   int dummy;

   sa1[0] = s1;
   sa2[0] = s2;

   // WCS projection routines require the input coordinates are in degrees
   // and in the range of [-90,90] for the lat and [-180,180] for the lon.
   // So correct for this effect.
   if(sa1[0] > 180) sa1[0] -= 360.;
   
   // Unknown what parameters 4 and 5 do.  Probably something about scaling.
   // Test later.
   prjs2x(&prj,1,1,1,1,sa1,sa2,xa1,xa2,&dummy);

   return std::make_pair<double,double>(xa1[0],xa2[0]);
}


/** @brief Convert from one projection to another
@param x1 projected equivalent to ra or l, in degrees
@param x2 projected equivalent dec or b, in degrees
@param projection used to deproject these coordinates
*/
std::pair<double,double> SkyProj::Project(double x1, double x2, SkyProj otherProjection)
{
   double s1, s2; // Sky Coordinates
   std::pair<double,double> s;
   s = otherProjection.Deproject(x1,x2);
   s1 = s.first;
   s2 = s.second;
   return SkyProj::Project(s1,s2);
}

/** @brief Does the inverse projection
@param x1 projected equivalent to ra or l, in degrees
@param x2 projected equivalent dec or b, in degrees
*/
std::pair<double,double> SkyProj::Deproject(double x1, double x2)
{
   double sa1[1], sa2[1];  // Sky Coordinates
   double xa1[1], xa2[1];
   int dummy;

   xa1[0] = x1;
   xa2[0] = x2;

   // Unknown what parameters 4 and 5 do.  Probably something about scaling.
   // Test later.
   prjx2s(&prj,1,1,1,1,xa1,xa2,sa1,sa2,&dummy);

    //fold RA into the range (0,360)
    while(sa1[0] < 0) sa1[0] +=360.;
    while(sa1[0] > 360) sa1[0] -= 360.;

   return std::make_pair<double,double>(sa1[0],sa2[0]);
}

/** @brief Change the projection type
@param projName String containing three char code
*/
void SkyProj::SetProjection(const std::string& projName)
{
   int i;
   bool code_found = false;

   // Valid Projection Codes
   const char prj_codes[26][4] =
   {"AZP", "SZP", "TAN", "STG", "SIN", "ARC", "ZPN", "ZEA", "AIR", "CYP",
   "CEA", "CAR", "MER", "COP", "COE", "COD", "COO", "SFL", "PAR", "MOL",
   "AIT", "BON", "PCO", "TSC", "CSC", "QSC"};

   for(i = 0; i < 26 && code_found != true; i++)
   {
      if(0 == strncmp(prj_codes[i],projName.c_str(),4))
      {
         code_found = true;
         strncpy(prj.code,projName.c_str(),4);
         prjset(&prj);
      }                  
   }

   if(code_found != true)
      throw Exception(std::string("Unrecognized SkyDir projection type: ")+projName);
}



