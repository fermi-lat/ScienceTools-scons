// $Header$

// Include files
#include "astro/SkyDir.h"

namespace astro {

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
            HepRotation galToCel = s_celestialToGalactic.inverse();
            //and do the transform to get the cartesian celestial vector
            m_dir = galToCel*gamgal;
            
        }else if(inputType == CELESTIAL){
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
        : m_dir(dir)
    {
        if(inputType!=EQUATORIAL){
            m_dir = s_celestialToGalactic.inverse() * m_dir;
        }
    }
    
    HepRotation SkyDir::s_celestialToGalactic = HepRotation().rotateZ(-282.25*M_PI/180).rotateX(-62.6*M_PI/180).rotateZ(33.*M_PI/180);
    
    void  SkyDir::setGalCoordsFromDir(double & l, double & b) const{
        
        //do the transform to get the galactic celestial vector
        Hep3Vector pointingin(s_celestialToGalactic*m_dir);
        
        // pointingin is the galactic cartesian pointing vector,
        //where yhat points at the galactic origin.
        // we want to make this into l and b now.
        l = atan2(pointingin.y(), pointingin.x())*180/M_PI;
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
    
    /*
    *  This routine will convert galactic longitude and latitude values ("l" and "b")
    *  into equivalent cartesian coordinates.  The values
    *  will be in internal Aitoff units where "l" is in the range [-2, 2]
    *  and "b" is in the range [-1, 1].
    */
    std::pair<double,double> SkyDir::hammerAitoff()const{
        double l = this->l();
        double b = this->b();
        float lover2, den;
        float radl, radb;
        
        //Force "l" to be in the range (-180 <= l <= +180) and
        //"b" to be in the range (-90 <= b <= +90).    
        while (l < -180.0) l += 360;
        while (l >  180.0) l -= 360;
        while (b <  -90.0) b += 180;
        while (b >   90.0) b -= 180;
        
        // Convert l and b to radians.
        double RadPerDeg = M_PI/180.;
        radl = l * RadPerDeg;
        radb = b * RadPerDeg;
        
        lover2 = radl / 2.0;
        den = sqrt(1.0 + (cos(radb) * cos(lover2)));
        l = 2.0 * cos(radb) * sin(lover2) / den;
        b = sin(radb) / den;
        // "x" is now in the range [-2, 2] and "y" in the range [-1, 1]. 
        
        return std::make_pair<double,double>(l,b);
    }
    
    
    
    
} //namespace astro
