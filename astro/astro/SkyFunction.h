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
    virtual ~SkyFunction(){}
protected:    
    SkyFunction(){}    
};
} //namesace astro

#endif
