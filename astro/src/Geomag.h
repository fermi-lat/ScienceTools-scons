/** \file Geomag.h
\brief declaration of Geomag

\author P. Nolan
$Header$

*/
#ifndef GEOMAG_H
#define GEOMAG_H

/** 
* \class GeoMag
*
* \brief //! Evaluate the geomagnetic variables (latitude, longitude, McIlwain L, B field) for any point in orbit. 
* Specialized for a low-inclination circular orbit at altitude 600 km.
* Method: Linear interpolation in tables with a 5-degree grid.  Latitude
* values between -30 and + 30 degrees.
* Data values obtained from the GSFC programs BILCAL and GEO_CGM.
* Latitudes and longitudes are both expressed in degrees.
* \author Patrick Nolan, Stanford University, February 2000.
* 
*/


namespace Geomag {
    double L(double lat, double lon);
    double B(double lat, double lon);
    double geolat(double lat, double lon);
    double geolon(double lat, double lon);
  
}

#endif //GEOMAG_H
