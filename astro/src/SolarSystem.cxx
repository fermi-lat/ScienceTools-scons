// $Header$

#include "astro/SolarSystem.h"

#include "SolSystem.h"
#include "jplephem/bary.h"

namespace astro {
SolarSystem::SolarSystem(): m_ss(new SolSystem){}

SolarSystem::SolarSystem( SolarSystem::Body body, JulianDate jd )
: m_ss(new SolSystem)
{
    direction(body, jd);

}

SkyDir SolarSystem::direction(Body body, JulianDate jd)
{
    m_ss->SetObj(body);
    m_ss->CalculatePos(jd);
    m_dir = SkyDir(m_ss->Ra*180/M_PI, m_ss->Dec*180/M_PI);

    return m_dir;
}
 
double SolarSystem::distance(Body body, JulianDate jd)
{
   m_ss->SetObj(body);
   m_ss->CalculatePos(jd);
   return m_ss->Edist;
}

// Returns an Hep3Vector with light seconds as distance units
Hep3Vector SolarSystem::getBarycenter(JulianDate jd)
{
   double jdt[2], *jdpointer;
   jdt[0] = floor(jd);
   jdt[1] = jd - floor(jd) - 0.5;
   int nearth = 3; 
   jdpointer = &jdt[0];

   int ephnum = 405;
   int denum;
   double c, radsol, msol;
   int j;
    if ( j = initephem (ephnum, &denum, &c, &radsol, &msol) ) {
      fprintf (stderr, "Error while initializing ephemeris; status: %d\n",
	       j) ;
      denum = 0 ;
    }

   const double *eposn =  dpleph(jdpointer, nearth, nearth);

   // Position of barycenter
   double x = -eposn[0];
   double y = -eposn[1];
   double z = -eposn[2];
//   double dist = sqrt(x*x + y*y + z*z);
//   double ra = atan2(y,x) * 180. / M_PI;
//   double dec = atan2(z,sqrt(x*x+y*y)) * 180. / M_PI;

   Hep3Vector barycenter;
   
   barycenter.setX(x);
   barycenter.setY(y);
   barycenter.setZ(z);
  
   return barycenter;
}

// Returns the velocity of the Earth with respect to the SSB
Hep3Vector SolarSystem::getEarthVelocity(JulianDate jd)
{
   double jdt[2], *jdpointer;
   jdt[0] = floor(jd);
   jdt[1] = jd - floor(jd) - 0.5;
   int nearth = 3; 
   jdpointer = &jdt[0];

   int ephnum = 405;
   int denum;
   double c, radsol, msol;
   int j;
    if ( j = initephem (ephnum, &denum, &c, &radsol, &msol) ) {
      fprintf (stderr, "Error while initializing ephemeris; status: %d\n",
	       j) ;
      denum = 0 ;
    }

   const double *eposn =  dpleph(jdpointer, nearth, nearth);

   Hep3Vector vearth;
 
   vearth.setX(eposn[3]);
   vearth.setY(eposn[4]);
   vearth.setZ(eposn[5]);

   // Convert from lightseconds/s to m/s
   vearth *= 299792458.;
  
   return vearth;
}

SolarSystem::~SolarSystem(){ delete m_ss;}
 
} // namespace astro
