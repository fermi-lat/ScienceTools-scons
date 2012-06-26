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
#include <cmath>

namespace {

    double EarthFlat= (1/298.25);            /* Earth Flattening Coeff. */
    double J2000= astro::JulianDate(2000,1,1,12); // 2451545.0;

    const double R2D = 180./M_PI;
    const double D2R = 1./R2D;
 
    inline double sqr(double x){return x*x;}

    // default polygon
    /* These points are assumed to be in counter-clockwise order,
       starting in the southeast */
//  This is the pre-launch polygon:
//    static double latv[]={-30,-26,-20,-17,-10, 1, 2, -3, -8,-12,-19,-30},
//                  lonv[]={ 45, 41, 31, 9,-11,-34,-46,-62,-79,-85,-89,-87};

//  Here is the polygon in use on-orbit:
    static double latv[] =  {-30.0, -22.6, 2.5, 5.2, 5.2, 4.6, 
        0.7, -8.6, -9.9, -12.5, -21.7, -30.0};
    static double lonv[] = { 33.9, 24.5, -18.6, -25.7, -36.0, -42.0, 
        -58.8, -93.1, -97.5, -98.5, -92.1, -86.1}; 

    // boundaries 
    static double lon_min = 1e10, lon_max = -1e10, lat_min = 1e10, lat_max=-1e10;
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
    if( m_lon<-M_PI) m_lon+=2*M_PI; // to get into (-180,180)
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
    if (m_haveMagCoords==false){
        m_L=0;
        m_B=0;
        m_lambda=0;
        m_geolat=0;
        m_R=0;
    }
}
   
void EarthCoordinate::computeMagCoords() const {
    // pointer to the singleton IGRField object for local use
    IGRField& field(IGRField::Model());
    // initialize it, and get what we will need. (can't use the object since it is a singleton
    double year( 2001 + m_met/3.15e7); // year does not need to be very precise

    field.compute(latitude(), longitude(), m_altitude, year);
    m_field = CLHEP::Hep3Vector(field.bEast(), field.bNorth(), -field.bDown());
    m_L = field.L();
    m_B = field.B();
    m_geolat = field.invariantLatitude();
    int signLambda = getHemisphere(longitude(), latitude());
    m_lambda = field.lambda()*signLambda;
    m_R      = field.R();

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
    //wrong double T1 = (24110.54841 + 8640184.812866 * T + 0.0093103 * T * T)/86400.0;
    double T1 = (24110.54841 + 8640184.812866*T + 0.093104*T*T - 0.0000062*T*T*T)/86400.0;

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
// Determine the magnetic hemisphere of the input lat/long point
//     returns 1 for "north", -1 for "south" 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   int EarthCoordinate::getHemisphere(double longitude, double latitude)const
{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // tabulated equator vs longitude (inspired by Jonathan Ormes)
    // goes between 0 and 360, 361 entries.

    // Average_equator, 2008.5-2014.5
    const double equator[361] = {
        9.12893, 9.12509, 9.12043, 9.12547, 9.10614, 9.0878, 9.06389, 9.04212, 9.01712, 8.98974, 
        8.94515, 8.90549, 8.89821, 8.91704, 8.79524, 8.63848, 8.56528, 8.47628, 8.3931, 8.33466, 
        8.19029, 8.13549, 8.01736, 7.89154, 7.7731, 7.68456, 7.50548, 7.2111, 7.06122, 6.90045, 
        6.74998, 6.75171, 6.62894, 6.56764, 6.49101, 6.38472, 6.45616, 6.32924, 6.25329, 6.16535, 
        6.01555, 6.01142, 5.97467, 5.91267, 5.87672, 5.84621, 5.80888, 5.74776, 5.73367, 5.72313, 
        5.74587, 5.75776, 5.79494, 5.82895, 5.86501, 5.88991, 5.90518, 5.97083, 6.05853, 6.15462, 
        6.24385, 6.315, 6.39317, 6.48312, 6.55668, 6.57505, 6.69768, 6.81947, 6.90898, 6.97184, 
        7.04992, 7.09266, 7.10825, 7.19904, 7.27216, 7.33849, 7.41377, 7.49186, 7.57323, 7.67057, 
        7.75715, 7.74055, 7.81247, 7.85565, 7.90756, 7.94283, 7.96711, 7.99988, 8.02581, 8.03955, 
        8.06666, 8.07404, 8.09071, 8.0157, 8.02011, 8.00772, 8.00339, 7.99407, 7.97944, 7.96386, 
        7.94389, 7.92571, 7.90614, 7.87856, 7.84593, 7.8079, 7.8003, 7.77482, 7.7524, 7.72225, 
        7.69631, 7.68072, 7.66355, 7.65676, 7.64937, 7.64809, 7.64966, 7.65818, 7.67318, 7.68811, 
        7.71161, 7.74108, 7.76851, 7.81393, 7.85904, 7.90586, 7.9535, 7.99533, 8.05112, 8.10576, 
        8.19435, 8.24303, 8.32835, 8.39499, 8.45072, 8.53796, 8.59237, 8.61051, 8.6479, 8.68144, 
        8.71582, 8.75173, 8.77522, 8.79975, 8.81652, 8.84711, 8.85437, 8.87049, 8.86699, 8.88616, 
        8.87212, 8.8547, 8.83754, 8.83804, 8.83019, 8.78842, 8.70286, 8.63788, 8.56541, 8.49992, 
        8.42118, 8.3236, 8.16402, 8.04416, 7.94171, 7.87034, 7.7385, 7.56339, 7.42605, 7.2441, 
        6.98916, 6.8694, 6.79479, 6.54844, 6.33535, 6.11434, 5.90066, 5.75717, 5.63005, 5.32392, 
        5.11443, 4.88111, 4.69059, 4.56186, 4.32411, 4.13411, 3.95294, 3.73912, 3.59185, 3.43233, 
        3.18693, 3.02458, 2.84572, 2.66121, 2.50816, 2.37178, 2.1557, 1.98322, 1.81926, 1.64733, 
        1.50305, 1.36135, 1.149, 0.977183, 0.814286, 0.634704, 0.48824, 0.337014, 0.119674, -0.0458091, 
        -0.264256, -0.406492, -0.583421, -0.757444, -0.970542, -1.14425, -1.36352, -1.52049, -1.69758, -1.9193, 
        -2.10364, -2.35513, -2.49765, -2.69008, -2.903, -3.09438, -3.32326, -3.48134, -3.67371, -3.85669, 
        -4.06824, -4.2609, -4.43484, -4.63901, -4.7796, -5.00986, -5.18888, -5.39854, -5.57602, -5.73069, 
        -5.9569, -6.13795, -6.33935, -6.52922, -6.67279, -6.91062, -7.08115, -7.27311, -7.49708, -7.63955, 
        -7.876, -8.07518, -8.28826, -8.49696, -8.65606, -8.91327, -9.16714, -9.35658, -9.53764, -9.78443, 
        -10.0357, -10.2426, -10.464, -10.7001, -10.9324, -11.1773, -11.4264, -11.6225, -11.8919, -12.1792, 
        -12.3885, -12.5971, -12.8705, -13.0753, -13.3513, -13.5601, -13.7435, -13.9779, -14.1698, -14.3875, 
        -14.5967, -14.7152, -14.8753, -14.9836, -15.0861, -15.1726, -15.2754, -15.3367, -15.3417, -15.347, 
        -15.2983, -15.248, -15.1454, -15.0225, -14.8746, -14.6912, -14.5154, -14.2814, -14.0262, -13.7023, 
        -13.3466, -12.9539, -12.4844, -11.9883, -11.4717, -10.9672, -10.3704, -9.80373, -9.22078, -8.63388, 
        -7.9686, -7.35731, -6.75172, -6.15741, -5.53153, -4.87315, -4.19436, -3.52165, -2.85605, -2.22433, 
        -1.58172, -0.991723, -0.379024, 0.235686, 0.786885, 1.36004, 1.87411, 2.40672, 2.8745, 3.37772, 
        3.82524, 4.23155, 4.64878, 5.01632, 5.3738, 5.73864, 6.01933, 6.33573, 6.62009, 6.86717, 
        7.11531, 7.32056, 7.52809, 7.7349, 7.93193, 8.09129, 8.21988, 8.33047, 8.43602, 8.56667, 
        8.65336, 8.76752, 8.83082, 8.9026, 8.95616, 9.0117, 9.04161, 9.09257, 9.09675, 9.11958, 
        9.12893
    };

    // parameters of the table
    const int    NBINS   = 361 ;
    const double maxLong = 360.;
    const double dTable  = 1.0 ;
    
    // some limits calculated during the table-generating process to speed the calculation
    const double minLat  = -16.347 ;
    const double maxLat  = 10.1289 ;
    const double cutLong = 206.5 ;

    enum hemi {SOUTH = -1, NORTH = 1, UNKNOWN = 0};
    
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // the easy part
    if(latitude>maxLat) return NORTH;
    if(latitude<minLat) return SOUTH;
    
    double long1 = longitude;

    // add/subtract multiples of 360 to put longitude into range 0-360

    if(long1<0 || long1>maxLong){
        double test = floor(long1/maxLong);
        long1 -= test*maxLong;
    } 
 
    // another big chunk
    //if(latitude<0&&long1<cutLong) return SOUTH;
    
   // find the bin
    int bin = (int) long1;
    // probably overkill, but cheap
    if(bin<0) bin = 0;
    if(bin>NBINS-2) bin = NBINS-2;

    double deltaLong = (long1 - bin)/dTable;
    double lat1 = equator[bin]*(1.-deltaLong) + equator[bin+1]*deltaLong;
    return (latitude>lat1 ? NORTH : SOUTH);
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
