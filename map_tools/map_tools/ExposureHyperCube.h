/** @file ExposureHyperCube.h
    @brief declare class ExposureHyperCube 

    @author Toby Burnett
    $Header$

*/

#ifndef TOOLS_EXPOSUREHYPERCUBE_H
#define TOOLS_EXPOSUREHYPERCUBE_H
#include "Exposure.h"
class BaseImage;

namespace map_tools {


/** @class ExposureHyperCube 
    @brief Set up an exposure map hypercube

    It is defined as a hypercube in ra, dec, sqrt(1-costheta) bins.

    */
class ExposureHyperCube  {
public:
    ExposureHyperCube( const Exposure& exp, std::string outfile)
        :m_exposure(exp),m_outfile(outfile)
    {
    }
    void save()
    {
        m_exposure.write(m_outfile);
    }

    ~ExposureHyperCube(){}
    const Exposure& m_exposure;
    std::string m_outfile;
};
}// namespace map_tools
#endif //TOOLS_EXPOSUREHYPERCUBE_H
