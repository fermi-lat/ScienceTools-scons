#ifndef OrbitModel_SkyProj_H
#define OrbitModel_SkyProj_H


// Include files
#include <cmath>
#include <utility> // for pair
#include <string>
#include <vector>
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
    public:
        ///Constructors
        /** @brief Default projection is Hammer-Aitoff
        */
        SkyProj();

        /** @brief Constructor specified by Projection Code
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
            **/
        SkyProj(const std::string &projName);

        /** @brief Constructor specified by Projection Code
            @param projName String containing three char code
            @param x0 reference x coordinate
            @param y0 reference y coordinate
            @param phi0 fiducial native longitude coordinate
            @param theta0 fiducial native latitude coordinate
            @param sxy XY Scaling?
            @param spt ?
            @param params vector of doubles containing any projection specific parameters
            **/
        SkyProj(const std::string &projName, double phi0, double theta0, double sxy, double spt, std::vector<double> params);

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
        /** @brief Change the projection type
            @param projName String containing three char code
        */
        void setProjection(const std::string& projName);


       /* Structure defined in WCSLIB prj.c.  This contains all
          projection information. */
        prjprm prj;

        /* Scaling parameters ? */
        double m_spt;
        double m_sxy;
        
    };


} // namespace astro
#endif    // LHCBEVENT_SKYPROJ_H
