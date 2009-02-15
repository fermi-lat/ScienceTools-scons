/** @file EarthCoordinate.cxx
    @brief implement class EarthCoordinate

 $Header$

*/
#include <cmath>


#include "astro/EarthCoordinate.h"
#include "Geomag.h"
#include "astro/IGRField.h"
#include <sstream>
#include <stdexcept>

namespace {

    double EarthFlat= (1/298.25);            /* Earth Flattening Coeff. */
    double J2000= astro::JulianDate(2000,1,1,12); // 2451545.0;

    const double R2D = 180./M_PI;
    const double D2R = 1./R2D;
 
    inline double sqr(double x){return x*x;}

    // default polygon
    /* These points are assumed to be in counter-clockwise order,
       starting in the southeast */
    static double latv[]={-30,-26,-20,-17,-10, 1, 2, -3, -8,-12,-19,-30},
                  lonv[]={ 45, 41, 31, 9,-11,-34,-46,-62,-79,-85,-89,-87};

    // boundaries 
    static double lon_min = 1e10, lon_max = 1e-10, lat_min = 1e10, lat_max=1e-10;
}

// static constants 
namespace astro {
double EarthCoordinate::s_EarthRadius = 6378145.; //m


double EarthCoordinate::earthRadius(){return s_EarthRadius;}

  std::vector<std::pair<double,double> >*  EarthCoordinate::s_SAA_boundary = 0;


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EarthCoordinate::EarthCoordinate( CLHEP::Hep3Vector pos, double met) 
   : m_met(met), m_haveMagCoords(false)
{
    using namespace astro;
    // check pos, modify to km if in m.
    if( pos.mag() >  s_EarthRadius) {
        pos /= 1e3;  // scale up to be presumably in km

    }

    if (s_SAA_boundary == 0 ) {
      s_SAA_boundary = new std::vector<std::pair<double, double> >;
    }
    m_lat = M_PI/2- pos.theta();
    // convert from MET to JD
    JulianDate jd(JulianDate::missionStart() + met/JulianDate::secondsPerDay);
    m_lon = pos.phi() - GetGMST(jd)*D2R;
    m_lon = fmod(m_lon, 2*M_PI); 
    if( m_lon<M_PI) m_lon+=2*M_PI; // for -180 to 180?
    if( m_lon>M_PI) m_lon-=2*M_PI;

    // oblateness correction to obtain geodedic latitude 
    m_lat=(atan(tan(m_lat))/(sqr(1.-EarthFlat)) );

    // this is also such a correction: the number 0.00669454 is the geodetic eccentricity squared?
    // see http://www.cage.curtin.edu.au/~will/gra64_05.pdf
    // or http://www.colorado.edu/geography/gcraft/notes/datum/gif/ellipse.gif
    m_altitude=sqrt(sqr(pos.x())+sqr(pos.y()))/cos(m_lat)
        -s_EarthRadius / (1000.*sqrt(1.-sqr(0.00669454*sin(m_lat))));

    if( fabs(m_altitude-550.) > 50){
        std::stringstream msg;
        msg <<"astro::EarthCoordinate: invalid altitude, expect near 550 km, got: " << m_altitude;
        throw std::invalid_argument(msg.str());
    }
}
   
void EarthCoordinate::computeMagCoords() const {
    // pointer to the singleton IGRField object for local use
    IGRField& field(IGRField::Model());
    // initialize it, and get what we will need. (can't use the object since it is a singleton
    double year( 2001 + m_met/3.15e7); // year does not need to be very precise

    // for getting the sign, until we find a better way...
    const double deltaLat = 1.0;
    field.compute(latitude()+deltaLat, longitude(), m_altitude, year);
    double newLambda = field.lambda();

    // normal computation
    field.compute(latitude(), longitude(), m_altitude, year);
    m_field = CLHEP::Hep3Vector(field.bEast(), field.bNorth(), -field.bDown());
    m_L = field.L();
    m_B = field.B();
    m_geolat = field.invariantLatitude();
    m_lambda = field.lambda();
    m_R      = field.R();

    double deltaLambda = newLambda - m_lambda;
    if(deltaLat>0&&deltaLambda<0) {
        m_lambda *= -1.0;
        m_geolat *= -1.0;
    }
    m_haveMagCoords = true;
}

double EarthCoordinate::L()const
{
   if (!m_haveMagCoords) {
      computeMagCoords();
   }
    return m_L; //Geomag::L(latitude(), longitude());

}
   
double EarthCoordinate::B()const
{
   if (!m_haveMagCoords) {
      computeMagCoords();
   }
    return m_B; // Geomag::B(latitude(), longitude());
}

double EarthCoordinate::R()const
{
   if (!m_haveMagCoords) {
      computeMagCoords();
   }
    return m_R; // radius;
}

double EarthCoordinate::lambda()const
{
   if (!m_haveMagCoords) {
      computeMagCoords();
   }
    return m_lambda*R2D;

}

double EarthCoordinate::geolat()const
{
   if (!m_haveMagCoords) {
      computeMagCoords();
   }
    //double old =Geomag::geolat(latitude(), longitude()); // for comparison
    return m_geolat*R2D; // convert to degrees  
}


double EarthCoordinate::geolon()const
{
    return Geomag::geolon(latitude(), longitude());
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double  EarthCoordinate::GetGMST(JulianDate jd)
{
    double J_D=jd;
    double M, Ora_Un_Dec=modf(J_D-0.5,&M)*24;  J_D-=Ora_Un_Dec/24;
    double T = (J_D - J2000) / 36525.;
    double T1 = (24110.54841 + 8640184.812866 * T + 0.0093103 * T * T)/86400.0;
    double Tempo_Siderale_0 = modf(T1,&M) * 24.;
    double Tempo_Siderale_Ora = Tempo_Siderale_0 + Ora_Un_Dec * 1.00273790935;
    if (Tempo_Siderale_Ora < 0.) Tempo_Siderale_Ora = Tempo_Siderale_Ora + 24.;
    if (Tempo_Siderale_Ora >= 24.) Tempo_Siderale_Ora = Tempo_Siderale_Ora - 24.;
    return Tempo_Siderale_Ora*15.;  
}  

void EarthCoordinate::setSAAboundary(const std::vector<std::pair<double,double> >& boundary)
{
    if (s_SAA_boundary == 0 ) {
      s_SAA_boundary = new std::vector<std::pair<double, double> >;
    }

    *s_SAA_boundary = boundary;
    lon_min = 1e10;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Determine whether we are inside the SAA (South Atlantic Anomaly)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool EarthCoordinate::insideSAA() const
{
    return insideSAA( latitude(), longitude());
}
bool EarthCoordinate::insideSAA(double lat, double lon) const
{
 
    typedef std::pair<double, double> coords;
    double my_lon(lon), my_lat(lat);
    
    if (s_SAA_boundary == 0 ) {
      s_SAA_boundary = new std::vector<std::pair<double, double> >;
    }

    if (s_SAA_boundary->size()==0)
    {
        // fill in from default
        for (size_t i = 0; i < sizeof(latv)/sizeof(double); ++i)
        {
            s_SAA_boundary->push_back(coords(latv[i], lonv[i]));
        }
    }
    if (lon_min==1e10) {
        for( std::vector<coords>::const_iterator it =s_SAA_boundary->begin();
            it!= s_SAA_boundary->end(); ++it)
        {
            double latv( it->first), lonv(it->second);
            if (latv < lat_min) lat_min = latv;
            if (latv > lat_max) lat_max = latv;
            if (lonv < lon_min) lon_min = lonv;
            if (lonv > lon_max) lon_max = lonv;
        }
    }
    
    // If outside rectangular boundary
    if (my_lon < lon_min || my_lon > lon_max || my_lat < lat_min || my_lat > lat_max)
        return false;
        
    /* Find nearest 2 boundary points to the east whose
        latitude straddles my_lat */
    std::vector<coords>::const_iterator it = s_SAA_boundary->begin();
    std::vector<coords>::const_iterator prev = it;
    for ( ; it->first < my_lat && it != s_SAA_boundary->end(); prev = it, ++it) {}
    
    if (it == s_SAA_boundary->end()) return false;
    
    // If my_lon is east of both boundary points
    if (it->second < my_lon && prev->second < my_lon)
        return false;
    
    // If my_lon is east of one of the two boundary points
    if (it->second < my_lon || prev->second < my_lon)
    {
        double slope = (it->first - prev->first)/(it->second - prev->second);
        double intersection = (my_lat - it->first)/slope + it->second;
        if (intersection < my_lon)
            return false;
    }
    
    if (it->first == my_lat)
    {
        prev = it;
        ++it;
    }
    
    /* So far so good.  Now find nearest 2 boundary points to the west whose
        latitude straddles my_lat */
    for ( ; it->first > my_lat && it != s_SAA_boundary->end(); prev = it, ++it) {}
    
    if (it == s_SAA_boundary->end()) return false;
    
    // If my_lon is west of both boundary points
    if (it->second > my_lon && prev->second > my_lon)
        return false;
    
    // If my_lon is west of one of the two boundary points
    if (it->second > my_lon || prev->second > my_lon)
    {
        double slope = (it->first - prev->first)/(it->second - prev->second);
        double intersection = (my_lat - it->first)/slope + it->second;
        if (intersection > my_lon)
            return false;
    }
    
    return true;
}
const CLHEP::Hep3Vector& EarthCoordinate::magnetic_field()const
{
    return m_field; 
}

double EarthCoordinate::latitude()const{ return m_lat*R2D;}
double EarthCoordinate::longitude()const{ return m_lon*R2D;}
double EarthCoordinate::altitude()const{ return m_altitude;}


} // namespace astro
