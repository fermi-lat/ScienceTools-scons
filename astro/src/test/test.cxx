// $Header$

#include <cassert>
#include "astro/GPS.h"
#include "astro/SolarSystem.h"
#include "astro/EarthCoordinate.h"
#include "astro/EarthOrbit.h"
#include "astro/JulianDate.h"
#include "astro/SkyDir.h"
#include "astro/PointingTransform.h"
#include "astro/HTM.h"
#include "astro/SkyProj.h"
#include "CLHEP/Vector/ThreeVector.h"

// local test classes
#include "TestHealpix.h"
#include "TestHealpixArray.h"

#include <stdexcept>

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
//------------------------------------------------------------
//          test code for SkyProj
//------------------------------------------------------------
/** @brief test a single point, transforming from world to pixel and back
*/
double test_one(double lon, double lat, const astro::SkyProj& t){
    using namespace astro;
    SkyDir dir(lon, lat);
    std::pair<double, double> pixel = dir.project(t );
  //  std::cout <<  "(" << lon << ", " << lat << ") \t-> (" << pixel.first << ", " << pixel.second << ") \n";
  //std::cout <<  "\t" << lon << "\t " << lat << "\t" << pixel.first << "\t" << pixel.second << "\n";
    return SkyDir(pixel.first,pixel.second,t).difference(dir);
}

/** @brief test a group of directions
*/
bool testSkyProj(){
    using namespace astro;

    // simple test of SkyProj
    //double crpix[]={180.5,90.5},  crval[]={0,0}, cdelt[]={-1,1}; // 1-degree CAR all sky
    double crpix[]={1,1},  crval[]={-179.5,-89.5}, cdelt[]={1,1}; // 1-degree CAR all sky
    std::string ctype("CAR");
    SkyProj proj(ctype, crpix, crval, cdelt);
    if( proj.isGalactic() ) throw std::runtime_error(" wrong return from SkyProj::isGalactic");

    // create another one to verify that it is possible to have more than one
    
    double zcrpix[]={180.5,90.5},  zcrval[]={0,0}, zcdelt[]={-1,1}; // 1-degree CAR all sky
    SkyProj other("AIT", zcrpix, zcrval, zcdelt,0, true);
    if( !other.isGalactic() ) throw std::runtime_error(" wrong return from SkyProj::isGalactic");

    double delta = 20;
    for( double dec =-90+delta/2; dec<90 ; dec+= delta){
        for( double ra = 0+delta/2; ra<360; ra+=delta){
            double test = test_one(ra, dec, proj);
            if( test > 1E-8){
                std::cout << "error - SkyProj transformation failed to invert" << std::endl;
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------
void test_insideSAA() {

    double lat_inSAA[] = {-30., -26., -20., -17., -10.,   1.,   2.,  -3.,  -8., -12., -19., -30., -28.,   0.,  -20., -25.,  -30};
    double lon_inSAA[] = { 45.,  41.,  31.,   9., -11.1, -34., -46., -62., -79., -85., -89., -87.,  43., -34., -20., -87.9, -20};

    double lat_notInSAA[] = {-30, -31, -26, -25, -19, -20, -16, -17, -9, -10,   2,   1,   3,   2,  -2,  -3,  -7,  -8, -11, -12, -18, -19, -31, -30};
    double lon_notInSAA[] = { 46,  45,  42,  41,  31,  32,   9,  10,-11, -10, -34, -33, -46, -47, -62, -63, -79, -80, -85, -86, -89, -90, -87, -88};

    std::cout << "\nTesting EarthCoordinate::insideSAA...";

    // Test for success
    for (int i = 0; i < sizeof(lat_inSAA)/sizeof(double); ++i) {
        astro::EarthCoordinate earthCoord(lat_inSAA[i], lon_inSAA[i]);
        assert(earthCoord.insideSAA());
    }

    // Test for failure
    for (int i = 0; i < sizeof(lat_notInSAA)/sizeof(double); ++i) {
        astro::EarthCoordinate earthCoord(lat_notInSAA[i], lon_notInSAA[i]);
        assert(!earthCoord.insideSAA());
    }
    std::cout << "Done.\n" << std::endl;
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
        std::cout << astro::JulianDate::missionStart().getGregorianDate() << std::endl;
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

#define ASSERT_EQUALS(X, Y) assert(fabs( (X - Y)/Y ) < 1e-5)

void checkdir(double ra1, double dec1, double ra2, double dec2) {
   astro::SkyDir dir1(ra1, dec1);
   astro::SkyDir dir2(ra2, dec2);
   assert(dir1.difference(dir2) < 1e-5);
}

bool test_GPS_readFitsData() {
   GPS * gps = GPS::instance();

   std::string filename("test_FT2.fits");
   gps->setPointingHistoryFile(filename);

   double time(30);
   gps->getPointingCharacteristics(time);

   ASSERT_EQUALS(gps->lat(), 28.675092697143555);
   ASSERT_EQUALS(gps->lon(), -75.576118469238281);

   checkdir(gps->RAX(), gps->DECX(), 281.15841674804688, 0.82335680723190308);
   checkdir(gps->RAZ(), gps->DECZ(), 11.064399719238281, -6.5129880905151367);
   checkdir(gps->RAZenith(), gps->DECZenith(), 
            11.605173110961914, 28.483125686645508);
   
   return true;
}

int main(){

    using namespace astro;
    using namespace std;
    int rc = 0;

    try {
        // test EarthCoordinate::insideSAA

 
// One needs the test data to run this.
//        if (!test_GPS_readFitsData()) return 1;

        JulianDate start = JulianDate::missionStart(); 

        //   testJD();

        double test=0;

        test += fabs(start -2451910.5);

        double ra=30,dec=50;
        SkyDir sd(ra, dec);

        double l=10.,b=-10.;
        SkyDir sd3(l,b,SkyDir::GALACTIC);

        test += fabs(l-sd3.l()) + fabs(b - sd3.b());
        test += fabs(ra-sd.ra()) +fabs(dec-sd.dec());

        EarthOrbit abcd;
        double juliandate = abcd.dateFromSeconds(0.0);


        std::cout << "EarthCoordinate:\n";
        EarthCoordinate xyza(abcd.position(juliandate),juliandate);
        
        std::cout << std::setprecision(6) << "\tlatitude at t0 = " << xyza.latitude()
            << " , longitude at t0 = " << xyza.longitude() << std::endl;

        double lat=25, lon=45;
        std::cout << "EarthCoordinate("<<lat<<","<<lon<<")\n";
        EarthCoordinate ec(lat, lon);

        test += fabs(lat-ec.latitude()) + fabs(lon-ec.longitude());
        double L = ec.L(), B=ec.B();
        std::cout << "\tL=\t"<< ec.L() 
            << "\n\tB=\t"<< ec.B() 
            << "\n\tgeolat=\t"<< ec.geolat() 
            << "\n\tgeolon=\t"<< ec.geolon() 
            << std::endl;

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

        // make sure can set ephemeris
        JulianDate jdbary(2454101.5001062094);
        SolarSystem ss(SolarSystem::Sun, juliandate);
        Hep3Vector bary = ss.getBarycenter(jdbary);
        std::cout << "Barycenter coords for JD: "
            << std::setprecision(8)<< jdbary << ": "  << bary << std::endl;
        // expected, from comparison with glbary.
        Hep3Vector expect (84.7056619, -445.132338, -192.922157);
        test += (expect-bary).mag() *1e3 ;


        /// @todo: some test of the barycenter calculation.
        /*
        // test projection (use default)

        std::pair<double,double> proj= sd.project();
        SkyDir sd4(proj.first, proj.second, astro::SkyDir::PROJECTION);
        //THB double sd4_ra= sd4.ra(), sd4_dec=sd4.dec();
        test += sd4.difference(sd);

        */

        if( fabs(test) < 1e-3 ) {
            cout << "tests ok " << endl;
            
        } else {
            cout << "failed a test" << endl;
            cout << "Mission start" << start << endl;  
            cout << "SkyDir("<<ra<<","<<dec<<") " << sd.ra() << ", " << sd.dec()   << endl;
            cout << "SkyDir3("<<l<<","<<b<<") " << sd3.l() << ", " << sd3.b()   << endl;
            cout << "EarthCoordinate("<<lat<<","<<lon<<") " << ec.latitude() << ", " << ec.longitude() << endl;

            // run the sun and moon
            rc= 1;
        }

        // other tests
        test_insideSAA();

        if( !testSkyDir() )rc=1;

        if( !testHTM() ) rc= 1;

        if(! testSkyProj() ) rc= 1;

        TestHealpix();
        TestHealpixArray();


    }catch( const std::exception& e){
        std::cerr << "Failed test because caught " <<typeid(e).name()<<" \""  
            << e.what() << "\"" << std::endl;
        rc= 1;
    }
    return rc;
}

