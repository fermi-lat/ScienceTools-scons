/** 
 * @file PowerLaw.h
 * @brief Declaration for the PowerLaw Function class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_PowerLaw_h
#define optimizers_PowerLaw_h

#include "optimizers/Function.h"
#include "optimizers/Arg.h"

namespace optimizers {
/** 
 * @class PowerLaw
 *
 * @brief A power-law function
 *
 */
    
class PowerLaw : public Function {

public:

   PowerLaw(double Prefactor=1, double Index=-2, double Scale=1);

   double integral(Arg & xmin, Arg & xmax) const;

   virtual Function * clone() const {
      return new PowerLaw(*this);
   }

protected:

   double value(Arg &) const;

   double derivByParamImp(Arg & x, const std::string & paramName) const;

};

} // namespace optimizers

#endif // optimizers_PowerLaw_h
