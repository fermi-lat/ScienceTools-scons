// $Header$

#include "astro/SolarSystem.h"
#include "astro/EarthCoordinate.h"
#include "astro/EarthOrbit.h"
#include "astro/JulianDate.h"
#include "astro/SkyDir.h"

bool testSkyDir(){
    using namespace astro;
    bool ok = true;
    for(double l = -175 ; l < 175 ; l += 5.){
        for(double b = -85 ; b < 85 ; b +=5.){
            SkyDir sd4(l,b,SkyDir::GALACTIC);
            double test = fabs(l-sd4.l()) + fabs(b - sd4.b());
            //uncomment next line for verbose output:
            //std::cout << "l in = " << l << " ,b in = " << b << 
             //   " ,l out = " << sd4.l() << " ,b out = " << sd4.b() << 
            //" ,ra out = " << sd4.ra() << " ,dec out = " << sd4.dec() << std::endl;
            if(test > 1E-3){ std::cout << "error - l,b output does not match input" << std::endl; 
            ok =false;}
        }
    }
    for(double ra = 5 ; l < 355 ; l += 5.){
        for(double dec = -85 ; dec < 85 ; dec +=5.){
            SkyDir sd4(ra,dec);
            double test = fabs(ra-sd4.ra()) + fabs(dec - sd4.dec());
            //uncomment next line for verbose output:
            //std::cout << "ra in = " << ra << " ,dec in = " << dec << 
            //    " ,ra out = " << sd4.ra() << " ,dec out = " << sd4.dec() << 
            //   " ,l out = " << sd4.l() << " ,b out = " << sd4.b() << std::endl;
            if(test > 1E-3){
                std::cout << "error - ra,dec output does not match input" << std::endl; 
                ok=false;
            }
        }
    }
    //test some known locations:
    SkyDir sd5(0.,0.,SkyDir::GALACTIC);
    //std::cout <<  "X =" << sd5.dir().x() << " , Y =" << sd5.dir().y() << " , Z =" << sd5.dir().z() << std::endl;
    
    std::cout << "galactic center corresponds to Ra = " << sd5.ra() << " , Dec = " << sd5.dec() << std::endl;
return ok;
}


int main(){

using namespace astro;
using namespace std;

    if( !testSkyDir() )return 1;

    JulianDate JD2000 = JulianDate(2000,1,1,12.); //2451545
    
    double test=0;

    test += fabs(JD2000 -2451545);

    double ra=30,dec=50;
    SkyDir sd(ra, dec);

    double l=-10.,b=-10.;
    SkyDir sd3(l,b,SkyDir::GALACTIC);

    test += fabs(l-sd3.l()) + fabs(b - sd3.b());
    test += fabs(ra-sd.ra()) +fabs(dec-sd.dec());
    
    double lat=40, lon=45;
    EarthOrbit abcd;
    double juliandate = abcd.dateFromSeconds(0.0);
    EarthCoordinate xyza(abcd.position(juliandate),juliandate);
    std::cout << "latitude at t0 = " << xyza.latitude()
        << " , longitude at t0 = " << xyza.longitude() << std::endl;
    

    EarthCoordinate ec(lat, lon);

    test += fabs(lat-ec.latitude()) + fabs(lon-ec.longitude());

    // test the SkyDir difference function
    SkyDir sd2(ra+1,dec);
    double 
        diff = sd2.difference(sd);
    test+= fabs( diff/cos(dec*M_PI/180) - M_PI/180. );

    if( test < 1e-3 ) {
        cout << "tests ok " << endl;
        return 0;
    }
    cout << "failed a test" << endl;
    cout << "JD2000" << JD2000 << endl;  
    cout << "SkyDir("<<ra<<","<<dec<<") " << sd.ra() << ", " << sd.dec()   << endl;
    cout << "SkyDir3("<<l<<","<<b<<") " << sd3.l() << ", " << sd3.b()   << endl;
    cout << "EarthCoordinate("<<lat<<","<<lon<<") " << ec.latitude() << ", " << ec.longitude() << endl;

    // run the sun and moon
    return 1; 
}

