/** 
 * @file RosenND.cxx
 * @brief Implementation for the ND Rosenbrock objective function
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <sstream>
#include <string>
#include <vector>

#include "optimizers/dArg.h"
#include "optimizers/ParameterNotFound.h"
#include "RosenND.h"

namespace optimizers {

RosenND::RosenND(int ndim, double prefactor) 
   : Statistic("RosenND", ndim), m_dim(ndim), m_prefactor(prefactor) {
   for (int i = 0; i < m_dim; i++) {
      std::ostringstream paramName;
      paramName << "x" << i;
      addParam(paramName.str(), 1, true);
   }
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

double RosenND::derivByParamImp(Arg &, 
                                const std::string & paramName) const {
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
