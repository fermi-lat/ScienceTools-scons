// $Header$

#include "astro/SolarSystem.h"

#include "SolSystem.h"

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

Hep3Vector SolarSystem::getBarycenter(JulianDate jd)
{
   //                       Mercury,  Venus,     Mars,      Jupiter,   Saturn,    Uranus,    Neptune,   Pluto,   Sun,       Moon
   const double mass[10] = {3.302e23, 4.8685e24, 6.4185e23, 1.8986e27, 5.6846e26, 8.6832e25, 1.0243e26, 1.25e22, 1.9891e30, 7.349e22  };

   double x = 0, y = 0, z = 0, totalMass = 0;
   for(int i = 0; i < 10; i ++)
   {
      m_ss->SetObj(i);
      m_ss->CalculatePos(jd);
      m_dir = SkyDir(m_ss->Ra*180/M_PI, m_ss->Dec*180/M_PI);
         
      x += mass[i] * (m_dir.dir()).x() * m_ss->Edist;
      y += mass[i] * (m_dir.dir()).y() * m_ss->Edist;
      z += mass[i] * (m_dir.dir()).z() * m_ss->Edist;
      totalMass += mass[i];      
   }

   Hep3Vector barycenter;
   barycenter.setX(x / totalMass);
   barycenter.setY(y / totalMass);
   barycenter.setZ(z / totalMass);
   
   return barycenter;
}

SolarSystem::~SolarSystem(){ delete m_ss;}
 
} // namespace astro
