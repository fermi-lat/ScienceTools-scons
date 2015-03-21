/** 
 * @file MyFun.h
 * @brief Test function declaration.
 * @author J. Chiang
 *
 * $Header$
 */

#include "optimizers/Function.h"

namespace optimizers {

class Arg;

/** 
 * @class MyFun
 *
 * @brief A simple test function that inherits from Function
 *
 */
    
class MyFun : public Function {

public:

   MyFun();

   ~MyFun(){}

   virtual Function * clone() const {
      return new MyFun(*this);
   }

protected:

   virtual double value(const Arg &) const;

   virtual double derivByParamImp(const Arg & x,
                                  const std::string & paramName) const;

};

} // namespace optimizers

