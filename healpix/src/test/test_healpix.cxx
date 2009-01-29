/** @file test_healpix.cxx
@brief Test program for various healpix routines

@author M. Roth, T. Burnett

$Header$
*/

#include "healpix/Map.h"
#include "src/base/message_error.h"
#include "healpix/HealPixel.h"
#include "TestHealpix.h"
#include "TestHealpixArray.h"
#include "facilities/commonUtilities.h"
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
using namespace std;
using namespace healpix;

int main() {

    int rc = 0;
    try {

        int i=6;//for(int i =6;i<=11;i++)
        //Map and filter test
        {
            stringstream oss;
            oss << i;
            string num(oss.str());
            cout << "Reading fits file "+num+"...";
            
            std::string filename=facilities::commonUtilities::joinPath(facilities::commonUtilities::getDataPath("healpix"), "srctest.fits"); 

            Map<double> mp(filename,i);
            cout << "done!" << endl;
            cout << "Filtering map...";
            mp.mfcn(155*(2*(i-5)),100*pow(2.35,(i-6)*1.));
            cout << "done!" << endl;
            //cout << "Writing output fits file...";
            //if the output file already exists, then the program will throw an error
            //mp.writemap(string("mfv_l"+num+".fits"));
            cout << "done!" << endl;
        }
        // HealPixel test
        {
            if( ! HealPixel::test() ) {
                ++rc; std::cerr << "Fail HealPixel test";
            }
            HealPixel h33=HealPixel(0,3);
            std::vector<HealPixel> neighbors = h33.neighbors();
            std::cout << "Neighbors of HealPixel(0,3): ";
            for( std::vector<HealPixel>::const_iterator it = neighbors.begin(); it!=neighbors.end(); ++it){
                std::cout << it->index() << ", ";
            }
            std::cout << endl;
        }

        TestHealpix();
        TestHealpixArray();

    }
    catch (const Message_error & error) {
        std::cout << error.what() << std::endl;
        return 1;
    }
    catch (const exception& e) {
        cerr << "Failed test because caught " <<typeid(e).name()<<" \""  
            << e.what() << "\"" << endl;
        rc= 1;
    }
    return 0;
}
