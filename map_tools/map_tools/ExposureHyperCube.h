/** @file ExposureHyperCube.h
    @brief declare class ExposureHyperCube 

    @author Toby Burnett
    $Header$

*/

#ifndef TOOLS_EXPOSUREHYPERCUBE_H
#define TOOLS_EXPOSUREHYPERCUBE_H

#include "image/Image.h"
#include "Exposure.h"

/** @class ExposureHyperCube 
    @brief Set up an exposure map hypercube

    It is defined as a hypercube in ra, dec, sqrt(1-costheta) bins.

    */
class ExposureHyperCube  {
public:
    ExposureHyperCube( const Exposure& exp, std::string outfile) : m_image(0)
    {

        std::vector<long> naxes(3);
        naxes[0]= Exposure::Index::ra_factor;
        naxes[1]= Exposure::Index::dec_factor;
        naxes[2] =Exposure::Index::cosfactor;

        m_image = new FloatImg("hypercube", outfile, naxes);
        m_image->data() = exp.data(); 

        setKey("TELESCOP", "GLAST");
        setKey("INSTRUME", "SIMULATION");
 
        setKey("DATE-OBS", "");
        setKey("DATE-END", "");
        setKey("EQUINOX", 2000.0);
        
        setKey("CTYPE1", "RA--CAR");
        setKey("CRPIX1",  0.5*(1+naxes[0]));
        setKey("CRVAL1",  0);
        setKey("CDELT1",  -1.);
        setKey("CUNIT1",  "deg");

        setKey("CTYPE2",  "DEC--CAR");
        setKey("CRPIX2",  0.5*(1+naxes[1]));
        setKey("CRVAL2",  0.);
        setKey("CDELT2",  1.0);
        setKey("CUNIT2",  "deg");

        setKey("CTYPE3",  "SQRT(1-COSTHETA)");
        setKey("CRPIX3",   0.5*(1+naxes[2]) );
        setKey("CRVAL3",  0.);
        setKey("CDELT3",  1./naxes[2] );
        setKey("CUNIT3",  "");

        setKey("BUNIT", "s");

        setKey("CROTA2",  0);
        // record total exposure as a key
        setKey("TOTAL", exp.total());

    }
    //! @brief add a string or douuble key to the image 
    void setKey(std::string name, double value, std::string unit="", std::string comment="")
    {
        m_image->addAttribute(DoubleAttr(name, value, unit, comment)); 
    }
    void setKey(std::string name, std::string value,std::string unit="", std::string comment="")
    {
        m_image->addAttribute(StringAttr(name, value,unit,comment)); 
    }
    void save(std::string outfile=""){
        m_image->saveElement(outfile);
        delete m_image;
        m_image=0;
    }
    ~ExposureHyperCube(){
        if(m_image!=0) save();
    }

private:
    FloatImg* m_image;

};
#endif //TOOLS_EXPOSUREHYPERCUBE_H
