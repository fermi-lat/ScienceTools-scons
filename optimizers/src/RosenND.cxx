/** 
 * @file RosenND.cxx
 * @brief Implementation for the ND Rosenbrock objective function
 * @author J. Chiang
 *
 * $Header$
 */

#include <vector>
#include <string>
#include <cmath>
#include <sstream>
#include "optimizers/dArg.h"
#include "RosenND.h"

namespace optimizers {

void RosenND::init() {
   int nParams = m_dim;
   setMaxNumParams(nParams);

   for (int i = 0; i < nParams; i++) {
      std::ostringstream paramName;
      paramName << "x" << i;
      addParam(paramName.str(), 1, true);
   }

   m_genericName = "RosenND";
   m_functionName = m_genericName;
}

double RosenND::value(Arg &) const {
   double my_value = 0;
   for (int i = 1; i < m_dim; i++) {
      double x = m_parameter[i-1].getTrueValue();
      double y = m_parameter[i].getTrueValue();
      my_value += m_prefactor*pow( (y - x*x), 2 ) + pow( (1. - x), 2 );
   }
   return -my_value;
}

double RosenND::derivByParam(Arg &, 
                             const std::string &paramName) const
   throw(ParameterNotFound) {

   std::vector<Parameter> params;
   getParams(params);

   for (unsigned int i = 0; i < params.size(); i++) {
      if (params[i].getName() == paramName) {
         if (i > 0 && i < params.size()-1) {
            double x = params[i-1].getTrueValue();
            double y = params[i].getTrueValue();
            double z = params[i+1].getTrueValue();
            return -( 2.*m_prefactor*(y - x*x) 
                      - 4.*m_prefactor*(z - y*y)*y 
                      - 2.*(1. - y) );
         } else if (i == 0) {
            double y = params[i].getTrueValue();
            double z = params[i+1].getTrueValue();
            return -( - 4.*m_prefactor*(z - y*y)*y 
                      - 2.*(1. - y) );
         } else if (i == params.size()-1) {
            double x = params[i-1].getTrueValue();
            double y = params[i].getTrueValue();
            return -( 2.*m_prefactor*(y - x*x) );
         }
      }
   }
   throw ParameterNotFound(paramName, getName(), "RosenND::derivByParam");
}

} // namespace optimizers
