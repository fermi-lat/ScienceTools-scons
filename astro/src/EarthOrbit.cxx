// $Header$

#include "astro/EarthOrbit.h"
#include "astro/EarthCoordinate.h"

#include <cmath>

namespace {
    
    double SecondsPerDay = 86400.;
    double R2D = 180/M_PI;
    double EarthFlat= (1/298.25);            /* Earth Flattening Coeff. */
    
    void range (double *s, double m)
    {
        while((*s)>=m) (*s)-=m;
        while((*s)<0) (*s)+=m;
    }
    
    
    inline double sqr(double x){return x*x;}
    
    astro::JulianDate JD_missionStart =astro::JulianDate(2005, 1, 1,0.0);
    astro::JulianDate JDStart =        astro::JulianDate(2005.,7,18,0.0);
}

// static constants relfecting orbit parameters

namespace astro {
double EarthOrbit::s_altitude = 550.e3  ; //m
double EarthOrbit::s_incl = 28.5*M_PI/180; //radian 
double EarthOrbit::s_e = 0.; //eccentricity

double EarthOrbit::s_radius = 6378145.; //m



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
EarthOrbit::EarthOrbit()
{
    initialize();
}

void EarthOrbit::initialize()
{
    double sini = sin(s_incl), cosi = cos(s_incl);
    static double J2=1.0822e-3;
    
    m_alt=(s_radius + s_altitude)/1000.; //altitude in km
    m_a = 1.0 + s_altitude/s_radius; 
    
    double T = M_PI/2*pow(m_a,1.5)/sqrt(5.98e24*6.67e-11)*pow(s_radius,1.5)  ; 	
    
    double u = 3.9860044e14/pow(s_radius,3)  ; 
    double n = sqrt(u / pow(m_a,3));
    double n1 = n*(1. + 3.*J2/2.*sqrt(1. - s_e*s_e)/(m_a*m_a)/pow((1. - s_e*s_e),2)*(1.-1.5*sini*sini));
    m_dwdt = n1*3.*J2/2./(m_a*m_a)/pow((1. - s_e*s_e),2)*(2. - 2.5*sini*sini);
    
    m_dOmegadt = -n1*3.*J2/2./(m_a*m_a)/pow((1. - s_e*s_e),2)*cosi;
    m_dMdt = n1;
    
    // phases for launch start
    // this is really the elapsed time in seconds since the MissionStart
    double StartSimDate = (JDStart-JD_missionStart) * SecondsPerDay;
    m_M0     = m_dMdt*StartSimDate;
    m_Omega0 = m_dOmegadt*StartSimDate;
    m_w0     = m_dwdt*StartSimDate;
    
    range(&m_M0,6.28); // should be 2pi
    
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Hep3Vector EarthOrbit::position(JulianDate JD) const
{
    double elapse = (JD - JDStart)*SecondsPerDay;
    
    double M=m_M0+m_dMdt*elapse;
    
    double Omega = m_Omega0+m_dOmegadt*elapse;
    
    // only for comparison with orbit.cpp -- should be 2pi
    range(&M,6.28);
    range(&Omega,6.28);
    
    double w = m_w0+m_dwdt*elapse;
    double Enew =Kepler(M,s_e); 
    
    
    Hep3Vector pos= Hep3Vector( cos(Enew)-s_e, sqrt(1.-sqr(s_e))*sin(Enew), 0 ).unit()*m_alt;
    pos.rotateZ(w).rotateX(s_incl).rotateZ(Omega);
    
    return pos;  
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
HepRotation EarthOrbit::CelestialToLocal(JulianDate JD) const
{
    double elapse = (JD - JDStart)*SecondsPerDay;
    
    double M=m_M0+m_dMdt*elapse;
    
    double Omega = m_Omega0+m_dOmegadt*elapse;
    
    // only for comparison with orbit.cpp -- should be 2pi
    range(&M,6.28);
    range(&Omega,6.28);
    
    double w = m_w0+m_dwdt*elapse; 
    
    HepRotation rot;
    
    rot.rotateZ(w).rotateX(s_incl).rotateZ(Omega);
    
    return rot;  
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double EarthOrbit::Kepler(double MeanAnomaly,double Eccentricity)
{
    double E = MeanAnomaly;    // Initial guess to Eccentric Anomaly
    if( Eccentricity==0) return E; // THB: skip following
    double Error;
    double TrueAnomaly;
    
    do
    {
        Error = (E - Eccentricity*sin(E) - MeanAnomaly)
            / (1. - Eccentricity*cos(E));
        E -= Error;
    }
    while (fabs(Error) >= 0.000001);
    
    if (fabs(E-M_PI) < 0.000001)
        TrueAnomaly = M_PI;
    else
        TrueAnomaly = 2.*atan(sqrt((1.+Eccentricity)/(1.-Eccentricity))
        *tan(E/2.));
    if (TrueAnomaly < 0)
        TrueAnomaly += 2.*M_PI;
    
    return TrueAnomaly;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
JulianDate EarthOrbit::dateFromSeconds(double seconds)const{
    return JDStart+(seconds/SecondsPerDay);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
double EarthOrbit::phase(JulianDate jd) const
{
    double elapse = (jd - JDStart)*SecondsPerDay;
    //double M=m_M0+m_dMdt*elapse;
    return m_Omega0+m_dOmegadt*elapse;
    
}

}
