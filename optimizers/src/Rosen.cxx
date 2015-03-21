/** 
 * @file Rosen.cxx
 * @brief Implementation for the 2D Rosenbrock objective function
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <string>
#include <vector>

#include "optimizers/dArg.h"
#include "optimizers/ParameterNotFound.h"
#include "Rosen.h"

namespace optimizers {

Rosen::Rosen(double prefactor) 
   : Statistic("Rosen", 2), m_prefactor(prefactor) {
   addParam("x", 1, true);
   addParam("y", 1, true);
}

double Rosen::value(const Arg &) const {
   double x = m_parameter[0].getTrueValue();
   double y = m_parameter[1].getTrueValue();

   return -(m_prefactor*pow((y - x*x), 2) + pow((1 - x), 2));
}

double Rosen::derivByParamImp(const Arg &, 
                              const std::string & paramName) const {
   std::vector<double> params;
   getParamValues(params);

   double x = params[0];
   double y = params[1];

   if (paramName == "x") {
      return -(-4.*m_prefactor*(y - x*x)*x - 2.*(1. - x));
   } else if (paramName == "y") {
      return -2.*m_prefactor*(y - x*x);
   }
   throw ParameterNotFound(paramName, getName(), "Rosen::derivByParam");
}

} // namespace optimizers
