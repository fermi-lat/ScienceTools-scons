//
//    GRBShell: Class that describes the a Shell
//    Authors: Nicola Omodei & Johann Cohen Tanugi 
//

#include "GRBShell.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

GRBShell::GRBShell(double gamma, double mass,double thickness, double radius) 
  : m_gamma(gamma), m_mass(mass),m_thickness(thickness),m_radius(radius)
{ 
}



double GRBShell::beta(const double gamma) 
{
  if(gamma<1.0)
    {
      return 0;
    }
  else 
    {
      return sqrt(1. - 1./(gamma*gamma));  
    }
}


void GRBShell::evolve(double dt) 
{
  //Interaction with the Inter Stellar Medium: 
  if(m_radius>1.0e+17)
    {
      m_gamma=cst::viscosity+m_gamma*(1.0-cst::viscosity);
    }
  if(m_gamma<1.0) m_gamma=1.0;
  m_radius += beta(m_gamma)*cst::c*dt;
   // Expanding sells...
  //  m_thickness=m_radius/pow(m_gamma,2)>m_thickness?m_radius/pow(m_gamma,2):m_thickness;
  
}
