/** @file Exposure.h
    @brief definition of the class Exposure

    @author T.Burnett
    $Header$
*/
#ifndef MAP_TOOLS_EXPOSURE_H
#define MAP_TOOLS_EXPOSURE_H

#include <vector>
#include <string>
#include <cmath>

namespace astro { class SkyDir;}
namespace tuple { class ITable;}
namespace { 
    inline double sqr(double x){return x*x;}
}

namespace map_tools {
/** @class Exposure
@brief Manage an exposure database.

It is a grid in (ra,dec) coordinates, with layers representing binning in cos(theta)

*/
class Exposure  {

public:
    /** @class Index 
    @brief key for indexing the histogram into ra,dec,costh bins
    */
    class Index{
    public:
        operator unsigned int() const{return m_index;}

        Index(double ra,double dec, double costheta=1.0){
            if( ra<0.) ra += 360.; if(ra>360.) ra-= 360.;
            int ra_bin =int(ra/skybinsize);
            int dec_bin=int((dec+90.)/skybinsize);
            int theta_bin = costheta_bin(costheta); //int(((costheta-cosmin)/costhetabinsize));
            m_index = static_cast<int>( ra_bin + ra_factor*(dec_bin + dec_factor*theta_bin) );
        }
        Index(int i) : m_index(i){}
#if 1 // version with weighted cos theta
        static int costheta_bin(double costheta){ 
            return static_cast<int>( sqrt((1.-costheta)/(1-cosmin))* cosfactor); 
        }
        static double costheta_value(int bin){ 
            return 1. - sqr((bin+0.5)/cosfactor)*(1.-cosmin); 
        }
        static std::string thetaBinning(){ return "SQRT(1-COSTHETA)";}
#else // uniform cos theta
        static int costheta_bin(double costheta){ 
            return static_cast<int>( (1.-costheta)/(1-cosmin)* cosfactor); 
        }
        static double costheta_value(int bin){ 
            return 1. - (bin+0.5)/cosfactor*(1.-cosmin); 
        }
        static std::string thetaBinning(){ return "COSTHETA";}
#endif

        // statics, define binning
        static double costhetabinsize;
        static double skybinsize;
        static int cosfactor, ra_factor, dec_factor;
        static double cosmin;
    private:
        unsigned int m_index;
    };

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //! Constructor 
    Exposure(double skybin=Index::skybinsize, double costhetabin=Index::costhetabinsize); 

    typedef   std::vector<float> ExposureCube;

    //! constructor from external source
    Exposure(const ExposureCube& cube, double total);

    //! constructor from a file
    Exposure(const std::string& filename);

    /** @class Exposure::Aeff

    @brief abstract base class to describe effective area as a funcion of the cosine of the polar angle in spacecraft coordinates
    */
    class Aeff {
    public:
        virtual double operator()(double costheta) const =0;
    };
    /// return the exposure at the given ra, dec, and cos theta weighting
    double operator()(double ra, double dec, const Aeff& fun)const;
    double operator()(const astro::SkyDir& dir, const Aeff& fun)const;

    //! total interval (sec)
    double total()const{return m_total;}

    //! access to the hypercube of exposure data
    const ExposureCube& data()const{return m_exposureMap;}

    /** @brief add a segment of the exposure history
       @param dirz the direction of the spacecraft z-axis
       @param deltat the time (presumeably seconds)
       */
    void add(const astro::SkyDir& dirz, double deltat);


private:   

    ExposureCube m_exposureMap;
    double m_total; //! total exposure
};
} // namespace map_tools
#endif
