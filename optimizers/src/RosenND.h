/** 
 * @file RosenND.h
 * @brief Declaration for a N-dimesional Rosenbrock objective function
 * @author J. Chiang
 *
 * $Header$
 */

#include "optimizers/Statistic.h"

namespace optimizers {
/** 
 * @class RosenND
 *
 * @brief A ND Rosenbrock test function
 *
 * @author J. Chiang
 *    
 * $Header$
 */
    
class RosenND : public Statistic {

public:

   RosenND(int ndim=3, double prefactor=100) : 
      m_dim(ndim), m_prefactor(prefactor) {init();}
      
   virtual double value(Arg&) const;

   virtual double value() const {
      Arg dummy;
      return value(dummy);
   }

   virtual double derivByParam(Arg &, const std::string &paramName) const;

   virtual void getFreeDerivs(std::vector<double> &derivs) const {
      Arg dummy;
      Function::getFreeDerivs(dummy, derivs);
   }

private:

   int m_dim;

   double m_prefactor;

   void init();

};

} // namespace optimizers

