/** @file ExposureHyperCube.h
@brief declare class ExposureHyperCube 

@author Toby Burnett
$Header$

*/

#ifndef TOOLS_EXPOSUREHYPERCUBE_H
#define TOOLS_EXPOSUREHYPERCUBE_H
#include "Exposure.h"
namespace tip { class Image; }

namespace map_tools {


    /** @class ExposureHyperCube 
    @brief Set up an exposure map hypercube

    It is defined as a hypercube in ra, dec, sqrt(1-costheta) bins.
    @todo: allow other binning function

    */
    class ExposureHyperCube  {
    public:
        ExposureHyperCube( const Exposure& exp, std::string outfile);

    private:
        //! pointer to the associated tip Image class
        tip::Image* m_image;
    };
}// namespace map_tools
#endif //TOOLS_EXPOSUREHYPERCUBE_H
