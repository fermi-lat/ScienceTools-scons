/** \file Geomag.cxx
\brief implementation of Geomag functions

$Header$

*/

#include "Geomag.h"
#include <iostream>
#include <stdexcept>

#include <cmath>

namespace {

    // table in anonymous namespace

#include "Geomag.inc"

    // Allowed Latitude range is -30 to +30
    // Allowed Longitude range is -180 to 180 (but 0 to 360 works too)
    double geoInterp(double lat, double lon, const double * array) {
        int ilat, ilon;
        double a, b;

        if(lon < 0)
            lon += 360;

        ilat = static_cast<int>(lat/5.+6);
        ilon = static_cast<int>(lon/5.);
        a = fmod(lat+30., 5.)/5.;
        b = fmod(lon, 5.)/5.;

        if(fabs(lat) > 30)
        {
            std::cout << "Warning -- Geomag::geoInterp -- Latitude is out of range."  << std::endl;
            throw std::invalid_argument("astro::Geomag::geoInterp -- Latitude is out of range");
        }

        return array[ilat   + 13*ilon    ] * (1.-a) * (1.-b) +
            array[ilat   + 13*(ilon+1)] * (1.-a) * b      +
            array[ilat+1 + 13*ilon    ] * a      * (1.-b) +
            array[ilat+1 + 13*(ilon+1)] * a      * b      ;
    }
}

double Geomag::L(double lat, double lon) {
    return geoInterp(lat, lon, Lvals);
}

double Geomag::B(double lat, double lon) {
    return geoInterp(lat, lon, Bvals);
}

double Geomag::geolat(double lat, double lon) {
    return geoInterp(lat, lon, glats);
}

double Geomag::geolon(double lat, double lon) {
    return geoInterp(lat, lon, glons);
}

