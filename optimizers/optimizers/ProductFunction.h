/** 
 * @file ProductFunction.h
 * @brief Declaration of ProductFunction class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_ProductFunction_h
#define optimizers_ProductFunction_h

#include "optimizers/CompositeFunction.h"

namespace optimizers {
/** 
 * @class ProductFunction
 *
 * @brief A Function that returns the product of two Functions
 *
 */
    
class ProductFunction : public CompositeFunction {
public:

   ProductFunction(Function & a, Function & b);

   virtual Function * clone() const {
      return new ProductFunction(*this);
   }

protected:

   double value(Arg &x) const {
      return m_a->operator()(x)*m_b->operator()(x);
   }

   void fetchDerivs(Arg &x, std::vector<double> &derivs, bool getFree) const;

};

} // namespace optimizers

#endif // optimizers_ProductFunction_h
