/** @file SolarSystem.cxx
@brief implementation of SolarSystem 


 $Header$
*/
#include "astro/SolarSystem.h"
#include "jplephem/bary.h" //local interface to the JPL ephemeris
#include <stdexcept>

namespace astro {

SolarSystem::SolarSystem(Body body)
: m_body(body)
{
}

SkyDir SolarSystem::direction(JulianDate jd)const
{
    return SkyDir(vector(m_body,EARTH,jd));
}
 
double SolarSystem::distance(JulianDate jd)const
{
   return vector(m_body,EARTH,jd).mag();
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

    static double jdt[2];
    double j0 = floor(jd+0.5);
    jdt[0]=j0; jdt[1]= jd-j0;
    return jdt;
}

// Returns an Hep3Vector with light seconds as distance units
Hep3Vector SolarSystem::getBarycenter(JulianDate jd)const
{
    const double *eposn =  dpleph(jplSetup(jd), m_body, SUN);

   // Position of barycenter
   double x = -eposn[0];
   double y = -eposn[1];
   double z = -eposn[2];
   return Hep3Vector(x,y,z);

}

Hep3Vector SolarSystem::getSolarVector(JulianDate jd)const
{
	return vector(EARTH,SUN,jd);
}

Hep3Vector SolarSystem::vector(Body targ, Body cent, JulianDate jd) {
   const double *eposn = dpleph(jplSetup(jd), targ, cent);
   
   // Position of targ with respect to the cent
   double x = -eposn[0] + eposn[6];
   double y = -eposn[1] + eposn[7];
   double z = -eposn[2] + eposn[8];

   return Hep3Vector(x,y,z);
}

SolarSystem::~SolarSystem(){
}
 
} // namespace astro
