/** 
 * @file ConstantValue.h
 * @brief Declaration for the ConstantValue Function class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_ConstantValue_h
#define optimizers_ConstantValue_h

#include "optimizers/Function.h"

namespace optimizers {

/** 
 * @class ConstantValue
 *
 * @brief This returns a constant value, the sole Parameter of this class,
 * regardless of the value or type of Arg.
 *
 */
    
class ConstantValue : public Function {

public:

   ConstantValue(double value=1) 
      : Function("ConstantValue", 1, "Value", "dArg", Addend) {
      addParam("Value", value, true);
   }

   virtual ~ConstantValue() {}

   virtual Function * clone() const {
      return new ConstantValue(*this);
   }

protected:

   double value(Arg &) const {
      return m_parameter[0].getTrueValue();
   }

   double derivByParamImp(Arg &, const std::string &) const {
      return m_parameter[0].getScale();
   }

};

} // namespace optimizers

#endif // optimizers_ConstantValue_h
