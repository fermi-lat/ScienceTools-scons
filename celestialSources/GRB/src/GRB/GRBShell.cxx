//
//    GRBShell: Class that describes the a Shell
//    Authors: Nicola Omodei & Johann Cohen Tanugi 
//

#include "GRBShell.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

GRBShell::GRBShell(double gamma, double mass,
		   double thickness, double radius) 
  : m_gamma(gamma), m_mass(mass),
    m_thickness(thickness),m_radius(radius)
{ 
}



double GRBShell::beta(const double gamma) 
{
  if(gamma<1.0)
    {
      cout << "warning: gamma undefined, returning beta=0" << endl;
      return 0;
    } else 
      {
	return sqrt(1. - 1./(gamma*gamma));  
      }
}


void GRBShell::evolve(double time) 
{
  m_radius += beta(m_gamma)*cst::c*time;
}
