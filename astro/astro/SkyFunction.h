/** @file SkyFunction.h

    @brief declare  the class SkyFunction
    @author Toby Burnett <tburnett@u.washington.edu>
    $Header$

*/

#ifndef ASTRO_SKYFUNCTION_H
#define ASTRO_SKYFUNCTION_H

#include "astro/SkyDir.h"

namespace astro {

/**
    @class SkyFunction
    @brief abstract base class for a functor that defines a function on the sky

*/
class SkyFunction 
{
public:

    //! @brief  coordinates of a point in the sky
    //! @return value at that point
    virtual double operator()(const astro::SkyDir& bincenter)const=0;

    //! @brief evaluate average over the opening angle, with tolerance
    //! Base class just returns the value -- meant for subclass to really determine
    virtual double average(const astro::SkyDir& dir, double /*angle*/, double /*tolerance*/)const{
        return (*this)(dir);
    }

    virtual ~SkyFunction(){}
protected:    
    SkyFunction(){}    
};
} //namesace astro

#endif
