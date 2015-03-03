/** 
 * @file PowerLaw.cxx
 * @brief Implementation for the PowerLaw Function class
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <string>
#include <vector>

#include "optimizers/dArg.h"
#include "optimizers/ParameterNotFound.h"

#include "PowerLaw.h"

namespace optimizers {

PowerLaw::PowerLaw(double Prefactor, double Index, double Scale) 
   : Function("PowerLaw", 3, "Prefactor", "dArg", Addend)  {
   addParam(std::string("Prefactor"), Prefactor, true);
   addParam(std::string("Index"), Index, true);
   addParam(std::string("Scale"), Scale, false);
}

double PowerLaw::value(Arg &xarg) const {
   double x = dynamic_cast<dArg &>(xarg).getValue();

   enum ParamTypes {Prefactor, Index, Scale};

   std::vector<Parameter> my_params;
   getParams(my_params);

   return my_params[Prefactor].getTrueValue()
      *pow((x/my_params[Scale].getTrueValue()), 
           my_params[Index].getTrueValue());
}

double PowerLaw::derivByParamImp(Arg & xarg,
                                 const std::string & paramName) const {
   double x = dynamic_cast<dArg &>(xarg).getValue();

   enum ParamTypes {Prefactor, Index, Scale};

   std::vector<Parameter> my_params;
   getParams(my_params);

   int iparam = -1;
   for (unsigned int i = 0; i < my_params.size(); i++) {
      if (paramName == my_params[i].getName()) iparam = i;
   }

   if (iparam == -1) {
      throw ParameterNotFound(paramName, getName(), "PowerLaw::derivByParam");
   }
   
   switch (iparam) {
   case Prefactor:
      if (my_params[Prefactor].getTrueValue() != 0) {
         return value(xarg)/my_params[Prefactor].getTrueValue()
            *my_params[Prefactor].getScale();
      } else {
         return pow((x/my_params[Scale].getTrueValue()), 
                    my_params[Index].getTrueValue())
            *my_params[Prefactor].getScale();
      }
      break;
   case Index:
      return value(xarg)*log(x/my_params[Scale].getTrueValue())
         *my_params[Index].getScale();
      break;
   case Scale:
      return -value(xarg)*(my_params[Index].getTrueValue())
         /(my_params[Scale].getTrueValue())
         *my_params[Scale].getScale();
      break;
   default:
      break;
   }
   return 0;
}

double PowerLaw::integral(Arg &xargmin, Arg &xargmax) const {
   double xmin = dynamic_cast<dArg &>(xargmin).getValue();
   double xmax = dynamic_cast<dArg &>(xargmax).getValue();

   enum ParamTypes {Prefactor, Index, Scale};
   std::vector<Parameter> my_params;
   getParams(my_params);

   double f0 = my_params[Prefactor].getTrueValue();
   double Gamma = my_params[Index].getTrueValue();
   double x0 = my_params[Scale].getTrueValue();

   return f0*x0/(Gamma+1.)*(pow((xmax/x0),Gamma+1.) - pow((xmin/x0),Gamma+1.));
}

} // namespace optimizers
