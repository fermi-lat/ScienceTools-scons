/** @file test_main.cxx
@brief test various classes

*/
#include "facilities/Util.h"
#include "astro/SkyDir.h"
#include "map_tools/Exposure.h"
#include "map_tools/Parameters.h"
#include "map_tools/ExposureHyperCube.h"
#include "map_tools/SkyImage.h"
#include "image/Header.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cassert>
using namespace map_tools;
class TestAeff : public Exposure::Aeff {
public:
    TestAeff(double slope=0): m_slope(slope){}
    double operator()(double ct)const{
//        std::cout << " theta = " << acos(ct)*180/M_PI << std::endl;
        return 1.-(1-ct)*m_slope;
    }
    double m_slope;
};

class TestPar : public Parameters {
public:
    TestPar(int argc, char** argv): Parameters(argc, argv){
        getValue<double>("xref");
    }
};

void test_Header();

int main(int argc, char** argv ){
   try{
        // read a pil file--and make sure that a few simple things work
        TestPar par(argc, argv);
        if( par["xref"] != 0) {
            std::cerr << "Read wrong value for parameter xref" << std::endl;
            return 1;
        }

        Exposure e( 10,  0.1);
        double total=0;

        // make a quick uniform cube.
        for( double ra=0.5; ra<360; ra+=2.0) {
            for (double st = -0.95; st < 1.0; st += 0.05){
                double dec = asin(st)*180/M_PI;
                e.add( astro::SkyDir(ra, dec), 1.0);
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

// Write out the cube...
        ExposureHyperCube cube(e, par.inputFile());
        cube.save();

// Check the Exposure(fitsfile) constructor.
        Exposure e2(par.inputFile());

// Write this out as a separate file for an external diff.
        ExposureHyperCube cube2(e2, par.outputFile());
        cube2.setKey("HISTORY", "This is a history header card.");
        cube2.setKey("COMMENT", "This is a comment header card.");
        cube2.save();

        // create an image to access cells
        SkyImage exp3(par.inputFile(),"hypercube");
        double tt = exp3.pixelValue(astro::SkyDir(0,0));
        assert(tt=36.0); 

        test_Header();

    }catch( const std::exception& e){
        std::cerr << "Failed test because caught " <<typeid(e).name()<<" \""  
            << e.what() << "\"" << std::endl;
        return 1;
    }
    std::cout << "tests OK" << std::endl;
    return 0;
}

void test_Header() {
   Header header;
   std::string name("my_attr");
   double dvalue0(3.14);
// Test for success.
   header.addAttribute(DoubleAttr(name, dvalue0));
   double value;
   header.getValue(name, value);
   assert(value == dvalue0);

// Test for failure of template instantiation.
   int ivalue;
   try {
      header.getValue(name, ivalue);
      assert(false);
   } catch (std::runtime_error &) {}

// Test for access failure.
   try {
      header.getValue("wrong name", value);
      assert(false);
   } catch (std::invalid_argument &) {}

// Test for re-insertion.
   float fvalue0(2.718F);
   header.addAttribute(FloatAttr(name, fvalue0));
   float floatValue;
   header.getValue(name, floatValue);
   assert(floatValue == fvalue0);

   header.addAttribute(IntAttr(name, 3), false);

   // test operator[]
   const BaseAttr & attr = header[name]; // should work
   std::string name_check(attr); 
   assert(name==name_check);
   try {
       const BaseAttr & attr = header["wrong"];//should not
       std::string name_check(attr); 
       assert(false);
   } catch (const std::runtime_error &){}

   float * test = reinterpret_cast<float*>(attr.valuePtr()); // check alternate return
   assert (*test == fvalue0);
   std::cout << "header tests ok\n";
}
