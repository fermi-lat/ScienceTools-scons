/** 
 * @file Rosen.h
 * @brief Declaration for a 2D Rosenbrock objective function
 * @author J. Chiang
 *
 * $Header$
 */

#include "optimizers/Function.h"

namespace optimizers {
/** 
 * @class Rosen
 *
 * @brief A 2D Rosenbrock test function
 *
 * @author J. Chiang
 *    
 * $Header$
 */
    
class Rosen : public Function {
public:

   Rosen() : m_prefactor(100) {init();}
   Rosen(double prefactor) : m_prefactor(prefactor) {init();}
      
   virtual double value(Arg&) const;

   virtual double derivByParam(Arg &, const std::string &paramName) const
      throw(ParameterNotFound);

private:

   double m_prefactor;

   void init();

};

} // namespace optimizers

