/** @file EarthCoordinate.h

*/
#ifndef astro_EarthCoordinate_H
#define astro_EarthCoordinate_H

#include "astro/JulianDate.h"

#include "CLHEP/Vector/ThreeVector.h"
namespace astro {
/** \class EarthCoordinate

  * \brief describe a point with respect to the surface of the Earth
  * \author T. Burnett and G. Tosti
  * <hr> $Id$
  *
  * Note that we calculate the geodetic coordinates: from http://ssd.jpl.nasa.gov/glossary.html#geodetic
  *
  *  Geodetic coordinates, latitude and longitude, specify a location on the Earth's oblate 
     (non-spherical) surface. Latitude, unless otherwise specified, is generally the geodetic latitude. 
      Geodetic latitude is defined as the angle between the equatorial plane 
     and a line normal to the surface at that location. 
     Geodetic longitude is the angular distance between the location's 
     meridian and the Greenwich meridian. 
  */
class EarthCoordinate   {
public:

    //! initialize with latitude and longitude, in deg, optional altitude 
    EarthCoordinate( double latDeg=0, double lonDeg=0 , double altitude=0);

    //! initialize with orbit position (in km), current date
    EarthCoordinate( Hep3Vector position, JulianDate jd);

    /** @brief true if inside the SAA

Some work on defining a realistic SAA boundary for the LAT is described at
http://www.slac.stanford.edu/~rac/SAA/
A contour plot of the SAA, showing a 12-segment polygon fit for the section of the SAA north of -30 degrees latitude is at http://www.slac.stanford.edu/~rac/SAA/saacode/saaplot.png

The (latitude,longitude) vertices for the SAA polygon are (in degrees):
latv=(-30,-26,-20,-17,-10, 1, 2, -3, -8,-12,-19,-30,-30);
lonv=( 45, 41, 31, 9,-11,-34,-46,-62,-79,-85,-89,-87, 45);

@TODO: implement this
    */
    bool insideSAA()const;

    //! the Earth radius in km
    static double earthRadius();

    double latitude()const;
    double longitude()const;
    double altitude()const;

    //!  McIlwain L
    double L()const;
    //! magnetic field in gauss
    double B()const;

    double geolat()const;///< geomagnetic latitude (deg)
    double geolon()const;///< geomagnetic longitude (deg)

private:

    //! internal representation: latitude and longitude in radians, altitude in km
    double m_lat, m_lon;
    double m_altitude;

   /**
     * GetGMST returns the Greenwich sideral time in degrees, 
     * The mean is: GMST is the angular distance between the 
     * Vernal point and the Greenwich meridian.
     */

    static double  GetGMST(JulianDate J_D);
    static double s_EarthRadius;
};



inline double EarthCoordinate::latitude()const{ return m_lat*180/M_PI;}
inline double EarthCoordinate::longitude()const{ return m_lon*180/M_PI;}
inline double EarthCoordinate::altitude()const{ return m_altitude;}

}
#endif
