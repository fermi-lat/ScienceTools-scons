/** @file SkyFunction.h

    @brief declare  the class SkyFunction
    @author Toby Burnett <tburnett@u.washington.edu>
    $Header$

*/

#ifndef MAP_TOOLS_SKYFUNCTION_H
#define MAP_TOOLS_SKYFUNCTION_H

namespace astro { class SkyDir; }

namespace map_tools {

/**
    @class SkyFunction
    @brief abstract base class for 

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
} //namesace map_tools

#endif
