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
 

SolarSystem::~SolarSystem(){ delete m_ss;}
 
} // namespace astro
