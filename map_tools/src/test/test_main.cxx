/** @file test_main.cxx
@brief test various classes

$Header$

*/
#include "map_tools/Exposure.h"
#include "map_tools/SkyImage.h"
#include "facilities/Util.h"
#include "hoops/hoops_prompt_group.h"

#include "TestCosineBinner.h"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cassert>
#include <stdexcept>
#include <typeinfo>
using namespace map_tools;

class TestAeff { 
public:
    TestAeff(double slope=0): m_slope(slope){}
    double operator()(double ct)const{
        //        std::cout << " theta = " << acos(ct)*180/M_PI << std::endl;
        return 1.-(1-ct)*m_slope;
    }
    double m_slope;
};




int main(int argc, char** argv ){
    int rc=0;
    try{

        // read a pil file--and make sure that a few simple things work
        hoops::ParPromptGroup par(argc, argv);
        double xref = par["xref"] ;
        if(  xref !=0 ) {
            std::cerr << "Read wrong value for parameter xref" << std::endl;
            return 1;
        }

        std::cout << "Testing exposure calculation with binning function "
            //  << Exposure::Index::thetaBinning() 
            << std::endl;
        Exposure e( 10,  0.1);
        double total=0;
#if 1
        // make a quick uniform cube.
        for( double ra=0.5; ra<360; ra+=2.0) {
            for (double st = -0.95; st < 1.0; st += 0.05){
                double dec = asin(st)*180/M_PI;
                e.fill( astro::SkyDir(ra, dec), 1.0);
                total += 1.0;
            }
        }
#else
        // this is a delta function at (0,0)
        e.fill(astro::SkyDir(0,0), 1.0); 
        total = 1.0;
#endif
#if 0
        double test = e(astro::SkyDir(0,0), TestAeff()) / total;
        if ( fabs(test-0.5)> 0.01 ){
            std::cerr << "bad cosine integral: " << test << std::endl;
            return 1;
        }
        test = e(astro::SkyDir(0,89), TestAeff()) / total;
        if ( fabs(test-0.5)> 0.01 ){
            std::cerr << "bad cosine integral: " << test << std::endl;
            return 1;
        }
        test = e(astro::SkyDir(180, 89), TestAeff()) / total;
        if ( fabs(test-0.5)> 0.01 ){
            std::cerr << "bad cosine integral: " << test << std::endl;
            return 1;
        }
        test = e(astro::SkyDir(0,0), TestAeff(1.0) ) /total;

        if ( fabs(test-0.25)> 0.01 ){
            std::cerr << "bad cosine integral: " << test << std::endl;
            return 1;
        }
#endif
        // Write out the cube...delete any existing file first.
        std::string infile(par["infile"].Value()), outfile(par["outfile"].Value());
        facilities::Util::expandEnvVar(&infile);
        facilities::Util::expandEnvVar(&outfile);
        e.write( infile);
        // Check the Exposure(fitsfile) constructor.
        Exposure e2(infile);

        // Write this out as a separate file for an external diff.
        e2.write(outfile);

        // now test cos
        TestCosineBinner();

        std::cout << "tests OK" << std::endl;

    }catch( const std::exception& e){
        std::cerr << "Failed test because caught " <<typeid(e).name()<<" \""  
            << e.what() << "\"" << std::endl;
        rc=1;
    }
    return rc;
}

