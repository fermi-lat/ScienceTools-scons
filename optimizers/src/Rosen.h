/** 
 * @file Rosen.h
 * @brief Declaration for a 2D Rosenbrock objective function
 * @author J. Chiang
 *
 * $Header$
 */

#include "optimizers/Statistic.h"

namespace optimizers {

/** 
 * @class Rosen
 *
 * @brief A 2D Rosenbrock test function
 *
 */
    
class Rosen : public Statistic {

public:

   Rosen(double prefactor=100);
      
   virtual double value() const {
      Arg dummy;
      return value(dummy);
   }

   virtual void getFreeDerivs(std::vector<double> & derivs) const {
      Arg dummy;
      Function::getFreeDerivs(dummy, derivs);
   }

   virtual Function * clone() const {
      return new Rosen(*this);
   }

protected:

   virtual double value(const Arg &) const;

   virtual double derivByParamImp(const Arg &,
                                  const std::string & paramName) const;

private:

   double m_prefactor;

};

} // namespace optimizers

