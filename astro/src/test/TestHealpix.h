/** @file TestHealpix.h
@brief code to test the class Healpix

*/

#include "astro/Healpix.h"
#include <algorithm>
#include <iomanip>



class TestHealpix {
public:
    TestHealpix(){
        using astro::Healpix;

        // create basic Healpix object to define the pixelization level nside
        Healpix hp(256, Healpix::RING);

        std::cout << "\nCreated a " << (hp.nested()? "NESTED":"RING") 
            << " Healpix object with " << hp.npix() << " pixels" 
            << " (nside="<<hp.nside()<<")"<< std::endl;

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
        for( int i =2; i<4; i+=2){
            double integral = hp.integrate(CosinePower(i))/(4.*M_PI),
                expect = ((i&1)==1)? 0 : 1./(i+1.);
            std::cout << i << "\t" 
                << std::setiosflags(4096) << std::setprecision(6)
                << std::setw(10) << integral
                << std::setw(15) << expect
                << std::setw(12) << std::setprecision(1)<< integral/expect -1
                << std::endl;
        }

    }


    class TestPixel {
    public:
        TestPixel(const astro::Healpix& hp):m_hp(hp){}

        void operator()(const astro::Healpix::Pixel& pix)
        {
            astro::SkyDir dir=pix; // behaves like a SkyDir
            double x = dir.dir().theta();
            double x2= dir.dir().phi();
            double theta = M_PI/2- pix().dec()*M_PI/180.;
            double phi = pix().ra()*M_PI/180;
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
