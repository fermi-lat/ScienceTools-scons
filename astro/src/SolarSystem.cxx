/** @file SolarSystem.cxx
@brief implementation of SolarSystem 


 $Header$
*/
#include "astro/SolarSystem.h"

#include "SolSystem.h"
#include "jplephem/bary.h" //local interface to the JPL ephemeris
#include <stdexcept>

namespace astro {
SolarSystem::SolarSystem(): m_ss(new SolSystem){
s_ephemInitialized = false;
}

SolarSystem::SolarSystem( SolarSystem::Body body, JulianDate jd )
: m_ss(new SolSystem)
{
    direction(body, jd);
   s_ephemInitialized = false;
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
   int nsun = 11;
   jdpointer = &jdt[0];

   if(!s_ephemInitialized) {
      int ephnum = 405;
      int denum;
      double c, radsol, msol;
      int j= initephem (ephnum, &denum, &c, &radsol, &msol);
      if ( j !=0 ) {
          throw std::runtime_error("SolarSytem::getBarycenter: could not initilze JPL ephemeris");
       }
      s_ephemInitialized = true;
   }

   const double *eposn =  dpleph(jdpointer, nearth, nsun);

   // Position of barycenter
   double x = -eposn[0];
   double y = -eposn[1];
   double z = -eposn[2];
//   double dist = sqrt(x*x + y*y + z*z);
//   double ra = atan2(y,x) * 180. / M_PI;
//   double dec = atan2(z,sqrt(x*x+y*y)) * 180. / M_PI;

   Hep3Vector barycenter(x,y,z);
   /*
   barycenter.setX(x);
   barycenter.setY(y);
   barycenter.setZ(z);
  */
   return barycenter;
}

Hep3Vector SolarSystem::getSolarVector(JulianDate jd)
{
   double jdt[2], *jdpointer;
   jdt[0] = floor(jd);
   jdt[1] = jd - floor(jd) - 0.5;
   int nearth = 3; 
   int nsun = 11;
   jdpointer = &jdt[0];

   if(!s_ephemInitialized) {
      int ephnum = 405;
      int denum;
      double c, radsol, msol;
      int j;
      if ( (j = initephem (ephnum, &denum, &c, &radsol, &msol))!=0 ) {
         fprintf (stderr, "Error while initializing ephemeris; status: %d\n",
	         j) ;
         denum = 0 ;
      }
      s_ephemInitialized = true;
   }

   const double *eposn =  dpleph(jdpointer, nearth, nsun);

   // Position of sun with respect to the geocenter
   double x = -eposn[0] + eposn[3];
   double y = -eposn[1] + eposn[4];
   double z = -eposn[2] + eposn[5];

    Hep3Vector solarVector(x,y,z);
   /*
   solarVector.setX(x);
   solarVector.setY(y);
   solarVector.setZ(z);
  */
   return solarVector;

}

SolarSystem::~SolarSystem(){ delete m_ss;}
 
} // namespace astro
