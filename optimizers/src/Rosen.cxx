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

void Rosen::init() {
   int nParams = 2;
   setMaxNumParams(nParams);

   addParam(std::string("x"), 1, true);
   addParam(std::string("y"), 1, true);
//   std::cout << "Rosen::init: " << m_parameter.size() << std::endl;

   m_genericName = "Rosen";
}

double Rosen::value(Arg &) const {
   double x = m_parameter[0].getTrueValue();
   double y = m_parameter[1].getTrueValue();
//   std::cout << "Rosen::value: " << m_parameter.size() << std::endl;

   return -(m_prefactor*pow((y - x*x), 2) + pow((1 - x), 2));
}

double Rosen::derivByParam(Arg &, 
                           const std::string &paramName) const {
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
