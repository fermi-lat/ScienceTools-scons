/** @file TestHealpix.h
@brief code to test the class Healpix

$Header$

*/

#include "astro/Healpix.h"
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <fstream>


// simple insertion operator to print a vector<int> object
std::ostream& operator<<(std::ostream& out, const std::vector<int> v)
{

    for (std::vector<int>::const_iterator it = v.begin(); it < v.end(); ++it)
    {
        out << *it << "\t";
    }
    out << std::endl;
    return out;
}

class TestHealpix {
public:
    TestHealpix(){
        using astro::Healpix;
        test(256, Healpix::NESTED, astro::SkyDir::GALACTIC);
        test(256, Healpix::NESTED, astro::SkyDir::EQUATORIAL);
        test(256, Healpix::RING, astro::SkyDir::GALACTIC);
        test(256, Healpix::RING, astro::SkyDir::EQUATORIAL);
        
        Healpix hp(8);
        TestNeighbors(hp);
    }
    void test(long nside, astro::Healpix::Ordering ord, astro::SkyDir::CoordSystem coord)
    {
        using astro::Healpix;

        // create basic Healpix object to define the pixelization level nside
        Healpix hp(nside, ord, coord);

        std::cout << "\nCreated a " << (hp.nested()? "NESTED":"RING") 
            << " Healpix object with " << hp.npix() << " pixels"  
            << " (nside="<<hp.nside()<<")"
            << " in " << (hp.galactic()? "GALACTIC":"EQUATORIAL") << " coords"<< std::endl;

#if 0
        // make a table of index, ra, dec 
        std::cout << "Table of index, ra, dec" << std::endl;
        std::copy(hp.begin(), hp.end() , std::ostream_iterator<Healpix::Pixel>(std::cout, "\n") );
        std::cout << "done" << std::endl;
#endif
        // test use of for_each, and apply test to each pixel
        std::cout << "\nTesting pixels...";
        std::for_each(hp.begin(), hp.end(), TestPixel(hp));
        std::cout << "done!"<< std::endl;
        // test doing an integral
        std::cout << "\nTesting integral with even powers of cos(theta)\n"
            << "n\tintegral/4PI\texpect\t\t relative error\n";
        for( int i =2; i<6; i+=2){
            double integral = hp.integrate(CosinePower(i))/(4.*M_PI),
                expect = ((i&1)==1)? 0 : 1./(i+1.);
            std::cout << i << "\t" 
                << std::setiosflags(std::ios_base::scientific) << std::setprecision(6)
                << std::setw(10) << integral
                << std::setw(15) << expect
                << std::setw(12) << std::setprecision(1)<< integral/expect -1
                << std::endl;
            if( fabs(integral-expect)> 1e-6) throw std::runtime_error("Failed integration test"); 

        }
    }


    class TestPixel {
    public:
        TestPixel(const astro::Healpix& hp):m_hp(hp){}

        void operator()(const astro::Healpix::Pixel& pix)
        {
            astro::SkyDir dir=pix; // behaves like a SkyDir
            astro::Healpix::Pixel p2(dir,m_hp);
            if( p2.index() != pix.index() ){
                throw std::runtime_error("index mismatch");
            }
        }
        const astro::Healpix& m_hp;
    };

    /// functor to try powers of cos(theta)
    class CosinePower : public astro::SkyFunction {
    public:
        CosinePower(int n): m_n(n){}
        double operator()(const astro::SkyDir& dir)const
        {
            double costheta = dir().z();
            return ::pow(costheta, m_n);
        }
        int m_n;
    };

    void TestNeighbors(const astro::Healpix & hp)
    {
        typedef std::map<long, long> SORTMAP;
        std::cout << "\nTesting neighbors logic...";
        std::ifstream f("../src/test/Healpix Neighbors Nside=8.txt"); // File provided by Healpix developers
           
        for (astro::Healpix::Iterator it = hp.begin(); it < hp.end(); ++it)
        {
            // std::cout << "Checking neighbors for pixel " << (*it).index() << std::endl;
            std::vector<astro::Healpix::Pixel> pv;
            (*it).neighbors(pv);  // Get neighbors as vector of pixels
            
            // Convert to vector of integers
            std::vector<int> calculated;
            for(std::vector<astro::Healpix::Pixel>::iterator it2 = pv.begin();
                it2 != pv.end(); ++it2)
            {
                calculated.push_back(it2->index());
            }
            
            // Get entry from file 
            int pixel_nbr;
            f >> pixel_nbr;  
            std::vector<int> from_file;
            for (int i = 0; i < 8; i++)
            {
                int neighbor;
                f >> neighbor;
                if (neighbor >= 0)
                    from_file.push_back(neighbor);
            }
            
            if (calculated != from_file)
            {
                std::cout << "Neighbor mismatch for pixel " << pixel_nbr << std::endl;
            }
            #if 0 // Run this block to see detailed results.
            else
            {
                std::cout << calculated << from_file << std::endl ;
            }
            #endif
        }
        std::cout << "Done.\n";
    }
};
