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
 * @author J. Chiang
 *    
 * $Header$
 */
    
class ConstantValue : public Function {

public:

   ConstantValue(double value=1) {
      setMaxNumParams(1);
      addParam("Value", value, true);

// Need to double-check these...
      m_funcType = Factor;
      m_argType = "";

      m_genericName = "ConstantValue";
   }

   virtual ~ConstantValue() {}

   double value(Arg&) const {return m_parameter[0].getTrueValue();}

   double derivByParam(Arg &, const std::string &) const
      {return 1;}

   virtual Function *clone() const {
      return new ConstantValue(*this);
   }

private:

   // disable this
   double integral(Arg &, Arg &) const {return 0;}

};

} // namespace optimizers

#endif // optimizers_ConstantValue_h
