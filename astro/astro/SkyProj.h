/** @file SkyProj.cxx
@brief declaration of the class SkyProj
$Header$
*/

#ifndef OrbitModel_SkyProj_H
#define OrbitModel_SkyProj_H


// Include files
#include <utility> // for pair
#include <string>

// forward declaration
struct wcsprm;

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
            @param crpix corresponds to the FITS keyword CRPIXi (coordinate reference point)
            @param crval corresponds to the FITS keyword CRVALi (coordinate value at reference point)
            @param cdelt corresponds to the FITS keyword CDELTi
            @param crota2 [default 0] corresponds to the FITS keyword CROTA2
            @param galactic if coords are to be interpreted as galactic
            **/
        SkyProj(const std::string &projName, 
                 double* crpix, double* crval, double* cdelt, double crota2=0, bool galactic=false  );

        // Destructor
       ~SkyProj();

       /** @brief tranform form world  to pixels with the given coordinates
       @param s1 ra or l, in degrees
       @param s2 dec or b, in degrees
       @return pair(x,y) in pixel coordinates
       */
        std::pair<double,double> sph2pix(double s1, double s2)const;

        /** @brief Convert from one projection to another
            @param x1 projected equivalent to ra or l, in degrees
            @param x2 projected equivalent dec or b, in degrees
            @param projection used to deproject these coordinates
        */
        std::pair<double,double> pix2pix(double x1, double x2, SkyProj otherProjection);

        /** @brief Does the inverse projection
            @param x1 projected equivalent to ra or l, in degrees
            @param x2 projected equivalent dec or b, in degrees
        */
        std::pair<double,double> pix2sph(double x1, double x2) const;

        /** @brief is this galactic? */
        bool isGalactic()const;
        class Exception; // forward declaration
    private:

        /* Structure defined in WCSLIB wcs.h.  This contains all
          projection information. */
       wcsprm *m_wcs;
    
    };


} // namespace astro
#endif    // LHCBEVENT_SKYPROJ_H
