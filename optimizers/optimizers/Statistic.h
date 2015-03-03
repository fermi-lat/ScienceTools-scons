/** 
 * @file Statistic.h
 * @brief Declaration of Statistic base class
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Statistic_h
#define optimizers_Statistic_h

#include <vector>

#include "optimizers/Function.h"

namespace optimizers {

/** 
 * @class Statistic
 *
 * @brief A base class for forcing Function objects to have a
 * convenient interface to use as objective functions, which take no
 * arguments.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class Statistic : public Function {
    
public:

   Statistic(const std::string & genericName, unsigned int maxNumParams) 
      : Function(genericName, maxNumParams, "", "dArg", None) {
   }

   virtual ~Statistic() {}

   virtual double value() const = 0;

   virtual void getFreeDerivs(std::vector<double> &derivs) const = 0;

protected:

   Statistic() : Function("Statistic", 0, "", "", None) {}

   Statistic(const Statistic &rhs) : Function (rhs) {}

};

} // namespace optimizers

#endif // optimizers_Statistic_h
