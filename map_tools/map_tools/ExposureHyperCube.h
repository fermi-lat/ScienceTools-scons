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
    ExposureHyperCube( const Exposure& exp, std::string outfile);
   
   //! @brief add a string or douuble key to the image 
   void setKey(std::string name, double value, std::string unit="", 
               std::string comment="");

   void setKey(std::string name, std::string value,
               std::string unit="", std::string comment="");

   void save(std::string outfile="");

   ~ExposureHyperCube();

private:
   BaseImage * m_image;

};
}// namespace map_tools
#endif //TOOLS_EXPOSUREHYPERCUBE_H
