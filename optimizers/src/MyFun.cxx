/** 
 * @file MyFun.cxx
 * @brief Implementation of a simple test function 
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
#include "MyFun.h"

namespace optimizers {

MyFun::MyFun() 
   : Function("MyFun", 3, "", "dArg", Addend) {
   addParam("Ruthie", 0.);
   addParam("Mary", 0.);
   addParam("Jane", 0.);
}

double MyFun::value(const Arg & xarg) const {
   double x = dynamic_cast<const dArg &>(xarg).getValue();

   double my_val(0);
   std::vector<Parameter> params;
   getParams(params);

   for (size_t i(0); i < params.size(); i++) {
      my_val += params[i].getTrueValue()*pow(x, int(i));
   }
   
   return my_val;
}

double MyFun::derivByParamImp(const Arg & xarg,
                              const std::string & paramName) const {
   double x = dynamic_cast<const dArg &>(xarg).getValue();

   std::vector<Parameter> params;
   getParams(params);

   for (unsigned int i = 0; i < params.size(); i++) {
      if (paramName == params[i].getName()) {
         return params[i].getScale()*pow(x, int(i));
      }
   }
   throw ParameterNotFound(paramName, getName(), "MyFun::deriveByParam");
}

} // namespace optimizers
