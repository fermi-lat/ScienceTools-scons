/** @file SkyDir.cxx
    @brief implementation of the class SkyDir

   $Header$
*/

// Include files

#include "astro/SkyDir.h"

using namespace astro;
#include <string>
#include <stdexcept>

using namespace CLHEP;

namespace {
    const double R2D = 180./M_PI;
    const double D2R = 1./R2D;
}


/** @brief initialize from (ra, dec), or (l,b)
@param param1 either ra or l, in degrees
@param param2 either dec or b, in degrees
@param inputType EQUATORIAL (default) or GALACTIC
*/
SkyDir::SkyDir(double param1, double param2, CoordSystem inputType){
    if(inputType == GALACTIC){
        double  l = param1*M_PI/180;
        double  b = param2*M_PI/180;

        //here we construct the cartesian galactic vector
        Hep3Vector gamgal( cos(l)*cos(b) , sin(l)*cos(b) , sin(b) );

        //get the transformation matrix from galactic to celestial coordinates.
        HepRotation galToCel = s_equatorialToGalactic.inverse();
        //and do the transform to get the cartesian celestial vector
        m_dir = galToCel*gamgal;

    }else if(inputType == EQUATORIAL){
        double ra = param1*M_PI/180;
        double dec = param2*M_PI/180;

        //here we construct the cartesian equatorial vector
        m_dir = Hep3Vector( cos(ra)*cos(dec), sin(ra)*cos(dec) , sin(dec) );        
    }else{
        //improper coordinate system declaration - default things and say so.
        throw std::invalid_argument("Improper coordinate System declaration in SkyDir" );
    }

}

/** @brief initialize from direction
*/
SkyDir::SkyDir(Hep3Vector dir, CoordSystem inputType)
: m_dir(dir.unit())
{
    if(inputType!=EQUATORIAL){
        m_dir = s_equatorialToGalactic.inverse() * m_dir;
    }
}

/** @brief initialize from projected coordinates

@param param1 projected equivalent to ra or l, in degrees
@param param2 projected equivalent dec or b, in degrees
@param projection used to deproject these coordinates
*/
SkyDir::SkyDir(double param1, double param2, const SkyProj& projection, bool check)
{
   // convert from pixel to ra/dec (or l/b) 
    if( check && projection.testpix2sph(param1, param2)!=0){
        m_dir=Hep3Vector(0,0,2);
        return;
    }
    // this can throw an exception
   std::pair<double,double> s = projection.pix2sph(param1, param2);

   double ra_rad = s.first * D2R;
   double dec_rad = s.second * D2R;

   Hep3Vector t = Hep3Vector( cos(ra_rad)*cos(dec_rad), sin(ra_rad)*cos(dec_rad) , sin(dec_rad) );        
   if( !projection.isGalactic()){
       // ctype1 specified RA
      m_dir = t;
   }else{
       // ctype1 specified GLON: convert to galactic
      m_dir = s_equatorialToGalactic.inverse()* t;
   }

}

HepRotation SkyDir::s_equatorialToGalactic 
= HepRotation()
    .rotateZ(-282.8592*M_PI/180)
    .rotateX(-62.8717 *M_PI/180)
    .rotateZ( 32.93224*M_PI/180);

void  SkyDir::setGalCoordsFromDir(double & l, double & b) const{

    //do the transform to get the galactic celestial vector
    Hep3Vector pointingin(s_equatorialToGalactic*m_dir);

    // pointingin is the galactic cartesian pointing vector,
    //where yhat points at the galactic origin.
    // we want to make this into l and b now.
    l = atan2(pointingin.y(), pointingin.x())*180/M_PI;
    if( l<0) l+=360;
    b = asin(pointingin.z())*180/M_PI;
}


double SkyDir::l ()const{
    double xl, xb; setGalCoordsFromDir(xl,xb); return xl;
}

double SkyDir::b ()const{
    double xl, xb; setGalCoordsFromDir(xl,xb); return xb;
}

double SkyDir::ra ()const{
    double ra=atan2(m_dir.y(), m_dir.x())*180/M_PI;    
    //fold RA into the range (0,360)
    while(ra < 0) ra+=360.;
    while(ra > 360) ra -= 360.;
    return ra;
}

double SkyDir::dec ()const{
    return asin(m_dir.z())*180/M_PI;
}

// wcslib based projection routine
std::pair<double,double> SkyDir::project(const SkyProj& projection) const
{
    if(projection.isGalactic()){
        double l, b; 
        setGalCoordsFromDir(l,b);
        return projection.sph2pix(l,b);
    } else {
      return projection.sph2pix(this->ra(),this->dec());
    }
}



double SkyDir::difference(const SkyDir& other)const
{
	double x = 0.5*(m_dir-other.dir()).mag();
        return 2.*asin(x);
}

        
std::pair<double, double> SkyDir::zenithCoords(const astro::SkyDir& zenithDir)const
{
    static Hep3Vector north_pole(0,0,1);
    Hep3Vector east_dir( north_pole.cross(zenithDir()).unit() ); // east is perp to north_pole and zenith
    Hep3Vector north_dir( zenithDir().cross(east_dir));

    double theta = difference(zenithDir); 
    double azimuth=atan2(dir().dot(north_dir),  dir().dot(east_dir)  ); // (y,x), so 0 is East, 90 North.
    if( azimuth <0) azimuth += 2*M_PI; // to 0-360 deg.
    // now convert to degrees
    azimuth *= 180/M_PI;
    theta *= 180/M_PI;
    return std::make_pair(azimuth, 90-theta); 
}

  
astro::SkyDir SkyDir::cross(const astro::SkyDir & other) const
{ 
         return astro::SkyDir(dir().cross(other.dir())); 
} 

double SkyDir::dot(const astro::SkyDir & other) const
{ 
         return dir().dot(other.dir()); 
} 
    




