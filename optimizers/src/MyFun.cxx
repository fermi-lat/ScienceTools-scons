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

/* implement MyFun as a polynomial of degree m_maxNumParams-1 */

MyFun::MyFun() {
   setMaxNumParams(3);
   addParam(std::string("Ruthie"), 0.);
   addParam(std::string("Mary"), 0.);
   addParam(std::string("Jane"), 0.);

// built-in unit test (justifies the existence of MyFun)
   try {
      addParam(std::string("Plain"), 3.14159);
   } catch (Exception &eObj) {
      std::cout << eObj.what() << std::endl;
   }
}

double MyFun::value(Arg &xarg) const {
   double x = dynamic_cast<dArg &>(xarg).getValue();

   double my_val = 0.;
   std::vector<Parameter> params;
   getParams(params);

   for (unsigned int i = 0; i < params.size(); i++) {
      my_val += params[i].getTrueValue()*pow(x, int(i));
   }
   
   return my_val;
}

double MyFun::derivByParam(Arg &xarg, const std::string &paramName) const {
   double x = dynamic_cast<dArg &>(xarg).getValue();

   std::vector<Parameter> params;
   getParams(params);

   for (unsigned int i = 0; i < params.size(); i++) {
      if (paramName == params[i].getName()) 
         return params[i].getScale()*pow(x, int(i));
   }
   throw ParameterNotFound(paramName, getName(), "MyFun::deriveByParam");
}

} // namespace optimizers
