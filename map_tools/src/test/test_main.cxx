/** @file test_main.cxx
@brief test various classes


*/

#include "map_tools/Exposure.h"
#include <iostream>
#include <algorithm>
#include <cassert>

class TestAeff : public Exposure::Aeff {
public:
    TestAeff(double slope=0): m_slope(slope){}
    double operator()(double ct)const{
//        std::cout << " theta = " << acos(ct)*180/M_PI << std::endl;
        return 1.-(1-ct)*m_slope;
    }
    double m_slope;
};

int main(int argc, char* argv[]){
    try{
        Exposure e( 10,  0.1);
        double total=0;

        // make a quick uniform cube.
        for( double ra=0.5; ra<360; ra+=1.0) {
            for (double st = -0.95; st < 1.0; st += 0.1){
                double dec = asin(st)*180/M_PI;
                e.findExposed( ra, dec, 1.0);
                total += 1.0;
            }
        }

        double test = e(0,0, TestAeff()) / total;
        if ( fabs(test-0.5)> 0.01 ){
            std::cerr << "bad cosine integral: " << test << std::endl;
            return 1;
        }
        test = e(0,89, TestAeff()) / total;
        if ( fabs(test-0.5)> 0.01 ){
            std::cerr << "bad cosine integral: " << test << std::endl;
            return 1;
        }
        test = e(180, 89, TestAeff()) / total;
        if ( fabs(test-0.5)> 0.01 ){
            std::cerr << "bad cosine integral: " << test << std::endl;
            return 1;
        }
        test = e(0,0, TestAeff(1.0) ) /total;

        if ( fabs(test-0.25)> 0.01 ){
            std::cerr << "bad cosine integral: " << test << std::endl;
            return 1;
        }


    }catch( const std::exception& e){
        std::cerr << "caught exception: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "tests OK" << std::endl;
    return 0;
}