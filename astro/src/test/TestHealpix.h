/** @file TestHealpix.h
@brief code to test the class Healpix

*/

#include "astro/Healpix.h"
#include <algorithm>
#include <iomanip>
#include <stdexcept>



class TestHealpix {
public:
    TestHealpix(){
        using astro::Healpix;
        test(256, Healpix::NESTED, astro::SkyDir::GALACTIC);
        test(256, Healpix::NESTED, astro::SkyDir::EQUATORIAL);
        test(256, Healpix::RING, astro::SkyDir::GALACTIC);
        test(256, Healpix::RING, astro::SkyDir::EQUATORIAL);
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


};
