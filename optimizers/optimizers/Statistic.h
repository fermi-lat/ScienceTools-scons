/** 
 * @file Statistic.h
 * @brief Declaration of Statistic class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Statistic_h
#define optimizers_Statistic_h

#include "optimizers/Function.h"

namespace optimizers {

class Arg;

/** 
 * @class Statistic
 *
 * @brief Abstract base class for objective functions used for
 * parameter estimation.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class Statistic : public Function {
    
public:

   virtual ~Statistic() {}

   /// Implement the Arg versions of these member functions relieving
   /// subclasses of this responsibility
   virtual double value(Arg &) const {return 0;}
   virtual double derivByParam(Arg &, const std::string &) const {return 0;}

   /// Return the objective function value taking the free parameters
   /// as the function argument
   virtual double value(const std::vector<double> &paramVec) = 0;
   virtual double operator()(const std::vector<double> &paramVec) 
      {return value(paramVec);}

   /// Non-argument version of getFreeDerivs
   virtual void getFreeDerivs(std::vector<double> &) = 0;

};

} // namespace optimizers

#endif // optimizers_Statistic_h
