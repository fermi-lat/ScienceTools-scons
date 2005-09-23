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
 * @author J. Chiang
 *    
 * $Header$
 */
    
class BrokenPowerLaw : public Function {

public:

   BrokenPowerLaw() {
      init(1, -1.5, -2.5, 1000.);
   }

   BrokenPowerLaw(double Prefactor, double Index1, double Index2, 
                  double BreakValue) {
      init(Prefactor, Index1, Index2, BreakValue);
   }

   double value(Arg &) const;

   double derivByParam(Arg & x, const std::string & paramName) const;

   virtual Function * clone() const {
      return new BrokenPowerLaw(*this);
   }

private:

   void init(double Prefactor, double Index1, 
             double Index2, double BreakValue);

// Disable this method.
   double integral(Arg &, Arg &) const 
      {return 0;}

};

} // namespace optimizers

#endif // optimizers_BrokenPowerLaw_h
