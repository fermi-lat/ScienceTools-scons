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
 * @author J. Chiang
 *    
 * $Header$
 */
    
class PowerLaw : public Function {
public:

   PowerLaw(){init(0, -2, 1);}
   PowerLaw(double Prefactor, double Index, double Scale)
      {init(Prefactor, Index, Scale);}

   double value(Arg&) const;

   double derivByParam(Arg &x, const std::string &paramName) const;

   double integral(Arg &xmin, Arg &xmax) const;

   virtual Function *clone() const {
      return new PowerLaw(*this);
   }

private:

   void init(double Prefactor, double Index, double Scale);

};

} // namespace optimizers

#endif // optimizers_PowerLaw_h
