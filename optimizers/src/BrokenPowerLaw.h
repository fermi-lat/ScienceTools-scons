/** 
 * @file BrokenPowerLaw.h
 * @brief Declaration for the BrokenPowerLaw Function class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_BrokenPowerLaw_h
#define optimizers_BrokenPowerLaw_h

#include "optimizers/Function.h"
#include "optimizers/Arg.h"

namespace optimizers {

/** 
 * @class BrokenPowerLaw
 *
 * @brief A broken power-law function
 *
 */
    
class BrokenPowerLaw : public Function {

public:

   BrokenPowerLaw(double Prefactor=1., double Index1=-1.5,
                  double Index2=-2.5, double BreakValue=1000.);

   virtual Function * clone() const {
      return new BrokenPowerLaw(*this);
   }

protected:

   double value(const Arg & xarg) const;

   double derivByParamImp(const Arg & xarg, 
                          const std::string & paramName) const;

};

} // namespace optimizers

#endif // optimizers_BrokenPowerLaw_h
