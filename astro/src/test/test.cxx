// $Header$

#include <cassert>
#include "astro/SolarSystem.h"
#include "astro/EarthCoordinate.h"
#include "astro/EarthOrbit.h"
#include "astro/JulianDate.h"
#include "astro/SkyDir.h"
#include "astro/PointingTransform.h"
#include "astro/HTM.h"
#include "CLHEP/Vector/ThreeVector.h"

bool testSkyDir(){
    using namespace astro;
    bool ok = true;
    double l;
    for(l = 5 ; l < 355 ; l += 5.){
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

void test_insideSAA() {

    int npts_inSAA = 19;
    double lon_inSAA[] = {-33.869, -54.191, -67.740, -81.288, -81.288, 
        -66.772, -49.353, -30.966, -10.643,   2.905,  
        26.131,  15.486,   0.970, -14.514, -25.159, 
        -20.320, -33.869, -51.288, -58.062};
    double lat_inSAA[] = {-4.912,  -4.912, -11.457, -15.821, -23.457, 
        -25.639, -24.548, -25.639, -26.730, -26.730, 
        -25.639, -21.275, -16.912, -12.548,  -9.275, 
        -18.003, -16.912, -16.912, -15.821};

    int npts_notInSAA = 49;
    double lon_notInSAA[] = { 54.196,  47.422,  34.841,  21.293,  10.647, 
        -1.933, -15.482, -28.062, -39.675, -55.159,
        -70.643, -81.288, -93.869, -92.901, 163.551,
        136.454,  79.357, 160.647, 128.712, 105.486,
        73.551,  42.583,  10.647, -12.579, -45.482,
        -75.482, -94.837, -110.320, -134.514, -163.546,
        -168.385, -144.191, -121.933, -103.546, -56.127,
        -19.353,  16.454,  50.325,  76.454, 102.583,
        121.938, 151.938, 169.357, -172.256, -152.901,
        -142.256, -119.998, -156.772, -157.740};
    double lat_notInSAA[] = { -25.639, -19.093, -15.821, -13.639, -10.366,
        -6.002,  -3.821,   2.725,   2.725,   3.816,
        -1.639,  -4.912, -13.639, -24.548, -20.184,
        -21.275, -23.457,  -2.730,  -2.730,  -3.821,
        -6.002,  -1.639,   8.179,   9.270,  10.361,
        9.270,   3.816, -15.821, -18.003, -18.003,
        -3.821,  -2.730,   2.725,  17.997,  23.452,
        23.452,  19.088,  14.725,  15.816,  13.634,
        13.634,  16.907,  21.270,  16.907,  23.452,
        13.634,  22.361,  11.452,   3.816};

    //   std::cout << "Testing EarthCoordinate::insideSAA" << std::endl;

    // Test for success
    for (int i = 0; i < npts_inSAA; i++) {
        astro::EarthCoordinate earthCoord(lat_inSAA[i], lon_inSAA[i]);
        assert(earthCoord.insideSAA());
    }

    // Test for failure
    for (int i = 0; i < npts_notInSAA; i++) {
        astro::EarthCoordinate earthCoord(lat_notInSAA[i], lon_notInSAA[i]);
        assert(!earthCoord.insideSAA());
    }
    std::cout << "EarthCoordinate::insideSAA tests passed." << std::endl;
}

void testJD()
{
    astro::JulianDate JD2000 = astro::JulianDate(2000,1,1,12.); //2451545

    int year, year2, month, month2, day, day2;
    double utc = 12.0, utc2;
    bool passed = true;

    for(year = 2002; year <= 2022; year++) {
        for(month = 1; month <= 12; month++) {
            for(day = 1; day < 28; day++)
            {
                for(utc = 0; utc < 24 && passed; utc=utc+1/3600.){
                    JD2000 = astro::JulianDate(year, month, day, utc);
                    JD2000.getGregorianDate(year2, month2, day2, utc2);

                    if(year-year2!=0 || month-month2!=0 || day-day2!=0)
                    {
                        std::cout << "Year, month, or day conversion error!" << std::endl;
                        std::cout << year << "  " << month << "  " << day << "   " << utc << std::endl;
                        std::cout << year2 << "  " << month2 << "  " << day2 << "   " << utc2 << std::endl;
                        passed = false;
                    }
                    if(utc-utc2 > 0.00000001)
                    {
                        std::cout << "Time error!"  << std::endl;
                        passed = false;
                    }
                    //               std::cout << JD2000.getGregorianDate() << std::endl;
                }
            }
        }
    }
    if(passed)
    {
        std::cout << "JD Conversions passed!" << std::endl;
        std::cout << JD2000.getGregorianDate() << std::endl;
    }
}
bool testHTM()
{
    using astro::HTM;
    size_t maxlevel = 6;
    astro::HTM h(maxlevel);
    //      h.dump(std::cout);
    for( size_t level = 0; level <= maxlevel; ++level) {
        double area = 0;
        for( HTM::const_iterator it = h.begin(level); it!=h.end(level); ++it){
            const HTM::Node & n = *it;
            area += n.area();
        }
        double check =  fabs(area/(4*M_PI) -1);
        if( check >1e-8) { 
            std::cout << "HTM area did not add up"<< std::endl; return false; 
        }
    }
    std::cout << "HTM check OK" << std::endl; 
    return true;
}

int main(){

    using namespace astro;
    using namespace std;

    // test EarthCoordinate::insideSAA

    test_insideSAA();

    if( !testSkyDir() )return 1;

    if( !testHTM() ) return 1;

    JulianDate JD2000 = JulianDate(2000,1,1,12.); //2451545

    //   testJD();

    double test=0;

    test += fabs(JD2000 -2451545);

    double ra=30,dec=50;
    SkyDir sd(ra, dec);

    double l=10.,b=-10.;
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

    //now test the galactic transformation function:
    SkyDir zenith(20,0,astro::SkyDir::GALACTIC);
    SkyDir xdir(-70,0,astro::SkyDir::GALACTIC);
    PointingTransform trans(zenith,xdir);
    Hep3Vector vertical(0,0,1);
    //double templ=trans.gDir(vertical).l();	
    //double tempb=trans.gDir(vertical).b();
    test += trans.gDir(vertical).l()-20.0;
    test += trans.gDir(vertical).b();

    // test projection (use default)

    std::pair<double,double> proj= sd.project();
    SkyDir sd4(proj.first, proj.second, astro::SkyDir::PROJECTION);
    //THB double sd4_ra= sd4.ra(), sd4_dec=sd4.dec();
    test += sd4.difference(sd);

    /*
    const char prj_codes[26][4] =
    {"AZP", "SZP", "TAN", "STG", "SIN", "ARC", "ZPN", "ZEA", "AIR", "CYP",
    "CEA", "CAR", "MER", "COP", "COE", "COD", "COO", "SFL", "PAR", "MOL",
    "AIT", "BON", "PCO", "TSC", "CSC", "QSC"};
    std::pair<double,double> newcoord;
    double weps;
    bool pass_low_lat, pass_mid_lat, pass_high_lat;
    cout << "Projection/Deprojection Closure Test (0.5 degree or better = pass)" << endl;
    cout << "Note: Some projections don't work at high latitudes and some projections" << endl;
    cout << "      can only project one hemisphere."  << endl;
    cout << "Projection   Pass High Lat    Pass Mid Lat    Pass Low Lat" << endl;
    for(int currentcode = 0; currentcode < 26; currentcode++)
    {
       pass_low_lat = true;
       pass_mid_lat = true;
       pass_high_lat = true;
       SkyProj Projection(prj_codes[currentcode]);
       for(int lat = -90; lat <= 90; lat++) {
          for(int lon = 0; lon <=360; lon++) {
             newcoord = Projection.project(lon,lat);
             newcoord = Projection.deproject(newcoord.first,newcoord.second);
             weps = newcoord.first - lon + newcoord.second - lat;
             if(weps > 0.5)
             {
                if(fabs((double) lat) > 60)
                   pass_high_lat = false;
                else if(fabs((double) lat) > 30)
                   pass_mid_lat = false;
                else
                   pass_low_lat = false;
             }
          }
       }
       cout << prj_codes[currentcode] << "\t\t" << pass_high_lat << "\t\t" << pass_mid_lat << "\t\t" << pass_low_lat << endl;
    }
    */

    if( fabs(test) < 1e-3 ) {
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

