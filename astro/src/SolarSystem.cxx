/** @file SolarSystem.cxx
@brief implementation of SolarSystem 


 $Header$
*/
#include "astro/SolarSystem.h"

#include "SolSystem.h"
#include "jplephem/bary.h" //local interface to the JPL ephemeris
#include <stdexcept>

namespace astro {

SolarSystem::SolarSystem()
: m_ss(new SolSystem)
{
}

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

double * SolarSystem::jplSetup(JulianDate jd)
{
    static bool ephemInit=false;
    if(!ephemInit) {
      int ephnum = 405;
      int denum;
      double c, radsol, msol;
      int j= initephem (ephnum, &denum, &c, &radsol, &msol);
      if ( j !=0 ) {
          throw std::runtime_error("SolarSytem::getBarycenter: could not initilze JPL ephemeris");
       }
      ephemInit = true;
   }
/*    the JPL takes 
 an array with two elements: a floored integer number of days since 
 noon -4712, and a fractional part between -0.5 and 0.5 which gets 
 added to produce any time between the midnight of the integer day to 
 the midnight of the next day.

                   jd[0]-.5      jd[0]       jd[0]+.5
                     mid          noon          mid

                   where -0.5 <= jd[1] < 0.5
*/

    static double jdt[]={ floor(jd+0.5), jd-jdt[0] };
    return jdt;
}

// Returns an Hep3Vector with light seconds as distance units
Hep3Vector SolarSystem::getBarycenter(JulianDate jd)
{
    enum {EARTHx=3, SUNx=11};
    const double *eposn =  dpleph(jplSetup(jd), EARTHx, SUNx);

   // Position of barycenter
   double x = -eposn[0];
   double y = -eposn[1];
   double z = -eposn[2];
   return Hep3Vector(x,y,z);

}

Hep3Vector SolarSystem::getSolarVector(JulianDate jd)
{
   enum {EARTHx=3, SUNx=11};

   const double *eposn =  dpleph(jplSetup(jd), EARTHx, SUNx);

   // Position of sun with respect to the geocenter
   double x = -eposn[0] + eposn[3];
   double y = -eposn[1] + eposn[4];
   double z = -eposn[2] + eposn[5];

   return Hep3Vector(x,y,z);
}

SolarSystem::~SolarSystem(){ delete m_ss;}
 
} // namespace astro
