#ifndef OrbitModel_SkyProj_H
#define OrbitModel_SkyProj_H


// Include files
#include <cmath>
#include <utility> // for pair
#include <string>
#include "wcslib/prj.h"

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
    private:
        prjprm prj;
    public:
        ///Constructors
        ///Default Constructor is for Hammer Aitoff Projection
        SkyProj();

        /** Constructor specified by Projection Code
            Valid codes are:
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
            **/
        SkyProj(const std::string &projName);

        // Parameter s1 corresponds to lon coordinate, s2 to lat coordinate 
        std::pair<double,double> Project(double s1, double s2);

        // Allows conversion from one projection to another
        // x1, x2 are lon and lat coordinates for the other projection
        std::pair<double,double> Project(double x1, double x2, SkyProj otherProjection);

        // Parameter x1 corresponds to lon coordinate, x2 to lat coordinate 
        std::pair<double,double> Deproject(double x1, double x2);

        void SetProjection(const std::string& projName);


        class Exception; // forward declaration
        
    };


} // namespace astro
#endif    // LHCBEVENT_SKYPROJ_H
