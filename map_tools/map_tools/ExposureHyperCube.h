/** @file ExposureHyperCube.h
    @brief declare class ExposureHyperCube 

    $Header$

*/

#ifndef TOOLS_EXPOSUREHYPERCUBE_H
#define TOOLS_EXPOSUREHYPERCUBE_H

#include "table/PrimaryHDU.h"
#include "Exposure.h"

/** @class ExposureHyperCube 
    @brief Set up an exposure map hypercube

    It is defined as a hypercube in ra, dec, sqrt(1-costheta) bins.

    */
class ExposureHyperCube : public table::PrimaryHDU<float> {
public:
    ExposureHyperCube( const Exposure& exp)
    {
        m_data = exp.data();

        std::vector<long> naxes(3);
        naxes[0]= Exposure::Index::ra_factor;
        naxes[1]= Exposure::Index::dec_factor;
        naxes[2] =Exposure::Index::cosfactor;
        Primary::setNaxis(naxes.size());
        Primary::setNaxes(naxes);
        Primary::setBitpix( table::Ifloat);
        Primary::setBufferSize(naxes[0]*naxes[1]*naxes[2]);
        Primary::setScale(1.0);

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

private:

};
#endif //TOOLS_EXPOSUREHYPERCUBE_H
