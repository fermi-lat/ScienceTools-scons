#ifndef OrbitModel_SkyProj_H
#define OrbitModel_SkyProj_H


// Include files
#include <cmath>
#include <utility> // for pair
#include <string>
#include <vector>
#include "wcslib/wcs.h"



namespace astro {


    /** @class SkyProj
    * @brief Map Projection wrapper class for WCSLIB
    * @author T. Hierath
    *
    * Note that units associated with sky coordinates (ra, dec, l, b) are consistently in degrees
    * ra and l are expected to be in the range [-90,90]
    * dec and b are expected to be in the range [0,360] 
    */

    class SkyProj
    {
    public:
        ///Constructors

        /** @brief Constructor specified by FITS parameters
            @param projName String containing three char code
            Valid codes are:
            @verbatim
            AZP: zenithal/azimuthal perspective 
            SZP: slant zenithal perspective 
            TAN: gnomonic 
            STG: stereographic 
            SIN: orthographic/synthesis 
            ARC: zenithal/azimuthal equidistant 
            ZPN: zenithal/azimuthal polynomial 
            ZEA: zenithal/azimuthal equal area 
            AIR: Airy 
            CYP: cylindrical perspective
            CEA: cylindrical equal area 
            CAR: Plate carree 
            MER: Mercator 
            SFL: Sanson-Flamsteed 
            PAR: parabolic 
            MOL: Mollweide 
            AIT: Hammer-Aitoff 
            COP: conic perspective 
            COE: conic equal area
            COD: conic equidistant
            COO: conic orthomorphic
            BON: Bonne
            PCO: polyconic
            TSC: tangential spherical cube
            CSC: COBE quadrilateralized spherical cube
            QSC: quadrilateralized spherical cube
            @endverbatim
            @param crpix1 corresponds to the FITS keyword CRPIX1 (coordinate reference point)
            @param crpix2 corresponds to the FITS keyword CRPIX2 (coordinate reference point)
            @param crval1 corresponds to the FITS keyword CRVAL1 (coordinate value at reference point)
            @param crval2 corresponds to the FITS keyword CRVAL2 (coordinate value at reference point)
            @param cdelt1 corresponds to the FITS keyword CDELT1
            @param cdelt2 corresponds to the FITS keyword CDELT2
            @param crota1 corresponds to the FITS keyword CROTA1
            @param crota2 corresponds to the FITS keyword CROTA2
            **/
        SkyProj(const std::string &projName, 
                 double crpix1,
                 double crpix2,
                 double crval1,
                 double crval2,
                 double cdelt1,
                 double cdelt2,
                 double crota1,
                 double crota2
                 );

        // Destructor
       ~SkyProj();

        /** @brief Do the projection with the given coordinates
            @param s1 ra or l, in degrees
            @param s2 dec or b, in degrees
        */
        std::pair<double,double> project(double s1, double s2);

        /** @brief Convert from one projection to another
            @param x1 projected equivalent to ra or l, in degrees
            @param x2 projected equivalent dec or b, in degrees
            @param projection used to deproject these coordinates
        */
        std::pair<double,double> project(double x1, double x2, SkyProj otherProjection);

        /** @brief Does the inverse projection
            @param x1 projected equivalent to ra or l, in degrees
            @param x2 projected equivalent dec or b, in degrees
        */
        std::pair<double,double> deproject(double x1, double x2);

        class Exception; // forward declaration
    private:

       /* Structure defined in WCSLIB wcs.h.  This contains all
          projection information. */
        wcsprm *wcs;
    
    };


} // namespace astro
#endif    // LHCBEVENT_SKYPROJ_H
