/** 
 * @file AbsEdge.cxx
 * @brief Implementation for the AbsEdge class that models an absorption 
 * edge spectral component
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>
#include <iostream>

#include <string>
#include <vector>

#include "optimizers/dArg.h"
#include "optimizers/ParameterNotFound.h"
#include "AbsEdge.h"

namespace optimizers {

AbsEdge::AbsEdge(double Tau0, double E0, double Index) 
   : Function("AbsEdge", 3, "", "dArg", Factor) {
   addParam(std::string("Tau0"), Tau0, true);
   addParam(std::string("E0"), E0, true);
   addParam(std::string("Index"), Index, true);
}

double AbsEdge::value(const Arg & xarg) const {
   double x = dynamic_cast<const dArg &>(xarg).getValue();

   enum ParamTypes {Tau0, E0, Index};

   std::vector<Parameter> my_params;
   getParams(my_params);

   if (x < my_params[E0].getTrueValue()) {
      return 1;
   }
   double tau = my_params[Tau0].getTrueValue()
      *pow(x/my_params[E0].getTrueValue(), my_params[Index].getTrueValue());
   return exp(-tau);
}

double AbsEdge::derivByParamImp(const Arg & xarg, 
                                const std::string & paramName) const {
   double x = dynamic_cast<const dArg &>(xarg).getValue();

   enum ParamTypes {Tau0, E0, Index};

   std::vector<Parameter> my_params;
   getParams(my_params);

   int iparam = -1;
   for (unsigned int i = 0; i < my_params.size(); i++) {
      if (paramName == my_params[i].getName()) iparam = i;
   }

   if (iparam == -1) {
      throw ParameterNotFound(paramName, getName(), "AbsEdge::derivByParam");
   }

   if (x > my_params[E0].getTrueValue()) {
      double my_value;
      double tau = my_params[Tau0].getTrueValue()
         *pow(x/my_params[E0].getTrueValue(), my_params[Index].getTrueValue());
      switch (iparam) {
      case Tau0:
         my_value = -value(xarg)*tau/my_params[Tau0].getTrueValue()
            *my_params[Tau0].getScale();
         return my_value;
         break;
      case E0:
         return value(xarg)*tau*my_params[Index].getTrueValue()
            /my_params[E0].getTrueValue()*my_params[E0].getScale();
         break;
      case Index:
         return -value(xarg)*tau*log(x/my_params[E0].getTrueValue())
            *my_params[Index].getScale();
         break;
      default:
         break;
      }
   }
   return 0;
}

} // namespace optimizers
