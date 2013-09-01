// $Heading:$

#include "astro/SolarSystem.h"
#include "astro/EarthCoordinate.h"
#include "astro/EarthOrbit.h"
#include "astro/JulianDate.h"
#include "astro/SkyDir.h"


int main(){

using namespace astro;
using namespace std;

    JulianDate JD2000 = JulianDate(2000,1,1,12.); //2451545
    
    double test=0;

    test += fabs(JD2000 -2451545);

    double ra=30,dec=50;
    SkyDir sd(ra, dec);

    test += fabs(ra-sd.ra()) +fabs(dec-sd.dec());
    
    double lat=40, lon=45;

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
    cout << "EarthCoordinate("<<lat<<","<<lon<<") " << ec.latitude() << ", " << ec.longitude() << endl;

    // run the sun and moon
    return 1; 
}