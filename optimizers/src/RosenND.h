/** 
 * @file RosenND.h
 * @brief Declaration for a N-dimesional Rosenbrock objective function
 * @author J. Chiang
 *
 * $Header$
 */

#include "optimizers/Function.h"

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
    
class RosenND : public Function {
public:

   RosenND(int ndim=3, double prefactor=100) : 
      m_dim(ndim), m_prefactor(prefactor) {init();}
      
   virtual double value(Arg&) const;

   virtual double derivByParam(Arg &, const std::string &paramName) const
      throw(ParameterNotFound);

private:

   int m_dim;

   double m_prefactor;

   void init();

};

} // namespace optimizers

