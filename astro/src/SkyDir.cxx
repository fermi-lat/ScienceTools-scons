/** @file SkyDir.cxx
    @brief implementation of the class SkyDir

   $Header$
*/

// Include files

#include "astro/SkyDir.h"

using namespace astro;
#include <string>
#include <stdexcept>


namespace{
        static double DEGTORAD=M_PI/180.;
}

class SkyDir::Exception : public std::exception 
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

        //here we construct the cartesian celestial vector
        m_dir = Hep3Vector( cos(ra)*cos(dec), sin(ra)*cos(dec) , sin(dec) );        
    }else{
        //improper coordinate system declaration - default things and say so.
        throw("Improper coordinate System declaration in SkyDir" );

        m_dir = Hep3Vector(0,0,1);
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
SkyDir::SkyDir(double param1, double param2, const SkyProj& projection, bool galactic)
{
   double ra_rad, dec_rad;
   std::pair<double,double> s;
   // Deproject the coordinates to find ra/dec (or l/b)
   s = projection.pix2sph(param1, param2);

   ra_rad = s.first * M_PI/180.;
   dec_rad = s.second * M_PI/180.;

   Hep3Vector t = Hep3Vector( cos(ra_rad)*cos(dec_rad), sin(ra_rad)*cos(dec_rad) , sin(dec_rad) );        
   if( !galactic){
      m_dir = t;
   }else{
      m_dir = s_equatorialToGalactic.inverse()* t;
   }

}

HepRotation SkyDir::s_equatorialToGalactic = HepRotation().rotateZ(-282.8592*M_PI/180).rotateX(-62.8717*M_PI/180).rotateZ(32.93224*M_PI/180);

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
std::pair<double,double> SkyDir::project(const SkyProj& projection, bool galactic) const
{
   if(galactic)
      return projection.sph2pix(this->l(), this->b());
   else
      return projection.sph2pix(this->ra(),this->dec());
}



double SkyDir::difference(const SkyDir& other)const
{
    // TODO: make this computationally efficient, avoid sqrt and asin at least for small angles
    return 2.*asin(0.5*(m_dir-other.dir()).mag());
}


  



