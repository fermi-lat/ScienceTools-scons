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
 * @author J. Chiang
 *    
 * $Header$
 */
    
class MyFun : public Function {
public:

   MyFun();
   ~MyFun(){}

   double value(Arg &) const;

   double derivByParam(Arg &x, const std::string &paramName) const;

protected:

   virtual MyFun * clone() const {
      return new MyFun(*this);
   }

private:

};

} // namespace optimizers

