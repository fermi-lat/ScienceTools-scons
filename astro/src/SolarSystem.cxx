/** @file SolarSystem.cxx
@brief implementation of SolarSystem 


 $Header$
*/
#include "astro/SolarSystem.h"
#include "jplephem/bary.h" //local interface to the JPL ephemeris
#include <sstream>

namespace astro {

SolarSystem::SolarSystem(Body body)
: m_body(body)
{
}

SkyDir SolarSystem::direction(JulianDate jd)const throw( SolarSystem::BadDate)
{
    return SkyDir(vector(EARTH, m_body,jd));
}

SkyDir SolarSystem::direction(JulianDate jd, const CLHEP::Hep3Vector& position)const
{
    static double c(299792.458); // velocity of light in km/s

    CLHEP::Hep3Vector r(vector(EARTH, m_body,jd));

    return SkyDir(r-position/c);
}


double SolarSystem::distance(JulianDate jd)const throw( SolarSystem::BadDate)
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
          throw BadDate("SolarSytem::getBarycenter: could not initilze JPL ephemeris");
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
CLHEP::Hep3Vector SolarSystem::getBarycenter(JulianDate jd)const throw( SolarSystem::BadDate)
{
    const double *eposn =  dpleph(jplSetup(jd), m_body, SUN);
    if( eposn==0) {
        std::stringstream msg;
        msg << "SolarSystem::getBarycenter called with bad Julian date " << jd;
        throw BadDate(msg.str());
    }

   // Position of barycenter
   double x = -eposn[0];
   double y = -eposn[1];
   double z = -eposn[2];
   return CLHEP::Hep3Vector(x,y,z);

}

CLHEP::Hep3Vector SolarSystem::getSolarVector(JulianDate jd)const throw( SolarSystem::BadDate)
{
	return vector(EARTH,SUN,jd);
}

CLHEP::Hep3Vector SolarSystem::vector(Body targ, Body cent, JulianDate jd) throw( SolarSystem::BadDate) 
{
   const double *eposn = dpleph(jplSetup(jd), targ, cent);
     if( eposn==0) {
        std::stringstream msg;
        msg << "SolarSystem::getBarycenter called with bad Julian date " << jd;
        throw BadDate(msg.str());
    }
  
   // Position of targ with respect to the cent
   double x = -eposn[0] + eposn[6];
   double y = -eposn[1] + eposn[7];
   double z = -eposn[2] + eposn[8];

   return CLHEP::Hep3Vector(x,y,z);
}

SolarSystem::~SolarSystem(){
}
 
} // namespace astro
