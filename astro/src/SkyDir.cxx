// $Header$

// Include files
#include "astro/SkyDir.h"

namespace astro {
SkyDir::SkyDir(double param1, double param2, CoordSystem inputType){
    if(inputType == GALACTIC){
       double  l = param1*M_PI/180;
       double  b = param2*M_PI/180;
        
        
        //here we construct the cartesian galactic vector
       Hep3Vector gamgal( cos(l)*cos(b) , sin(l)*cos(b) , sin(b) );

        //get the transformation matrix from galactic to celestial coordinates.
        HepRotation galToCel=celToGal().inverse();
        
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

SkyDir::SkyDir(Hep3Vector dir):
m_dir(dir){
}

HepRotation SkyDir::s_celestialToGalactic = HepRotation(-282.25*M_PI/180, -62.6*M_PI/180, 33*M_PI/180);

const HepRotation& SkyDir::celToGal(){
    //gal is the matrix which rotates (cartesian)celestial coordiantes into (cartesian)galactic ones
    static HepRotation gal;
    double degsPerRad = 180./M_PI;
    gal.rotateZ(-282.25/degsPerRad).rotateX(-62.6/degsPerRad).rotateZ(33./degsPerRad);
    return gal; 
}


std::pair<double,double> SkyDir::setGalCoordsFromDir() const{

    //get the transformation matrix from celestial to galactic coordinates.
    HepRotation celToGal(celToGal());
    
    //and do the transform to get the galactic celestial vector
    Hep3Vector pointingin(celToGal*m_dir);
     
    // pointingin is the galactic cartesian pointing vector,
    //where yhat points at the galactic origin.
    // we want to make this into l and b now.
    double l = atan2(pointingin.y(), pointingin.x());
    double b = asin(pointingin.z());
    
    l *= 360./M_2PI;
    b *= 360./M_2PI;
    
    return std::make_pair<double,double>(l,b);
}



double SkyDir::l ()const{
    return setGalCoordsFromDir().first;
}

double SkyDir::b ()const{
    return setGalCoordsFromDir().second;
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

} //namespace astro