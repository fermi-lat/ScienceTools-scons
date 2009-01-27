/** @file CosineBinner.h
@brief Define the CosineBinner classixel 

@author T. Burnett

$Header$
*/

#ifndef healpix_CosineBinner_h
#define healpix_CosineBinner_h

#include <vector>
#include <string>

namespace healpix {

    /** @class CosineBinner
        @brief manage a set of bins in cos(theta), and optioinally phi as well

    */

class CosineBinner : public std::vector<float> {
public:
    CosineBinner();
    
    /// the binning function: add value to the selected bin, if costheta in range
    void fill(double costheta, double value);
    
    /// the binning function: add value to the selected bin, if costheta in range
    //! alternate version for phi binning.
    void fill(double costheta, double phi, double value);
    
    /// modifiable reference to the contents of the bin containing the cos(theta) value
    float& operator[](double costheta);
    const float& operator[](double costheta)const;

    /// modifiable reference to the contents of the bin containing the cos(theta) value
    //! version that has phi as well (operator() allows multiple args)
    float& operator()(double costheta, double phi=-1);
    const float& operator()(double costheta, double phi=-1)const;

    /// cos(theta) for the iterator
    double costheta(std::vector<float>::const_iterator i)const;

    /// integral over the range with functor accepting costheta as an arg. 
    template<class F>
    double operator()(const F& f)const
    {   
        double sum=0;
        for(const_iterator it=begin(); it!=end(); ++it){
            sum += (*it)*f(costheta(it));
        }
        return sum; 

    }

    /// define the binning scheme with class (static) variables
    static void setBinning(double cosmin=0., size_t nbins=40, bool sqrt_weight=true);
    static void setPhiBins(size_t phibins);

    static std::string thetaBinning();
    static double cosmin();
    static size_t nbins();
    static size_t nphibins();


private:

    static double s_cosmin; ///< minimum value of cos(theta)
    static size_t s_nbins;  ///< number of costheta bins
    static bool  s_sqrt_weight; ///< true to use sqrt function, otherwise linear
    static size_t s_phibins; ///< number of phi bins
};

}
#endif
