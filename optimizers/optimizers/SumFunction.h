/** 
 * @file SumFunction.h
 * @brief Declaration of SumFunction class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_SumFunction_h
#define optimizers_SumFunction_h

#include "optimizers/CompositeFunction.h"

namespace optimizers {

/**
 * @class SumFunction
 *
 * @brief A Function that returns the linear sum of two Functions
 *
 */
    
class SumFunction : public CompositeFunction {
public:

   SumFunction(Function & a, Function & b);

   double integral(const Arg & xmin, const Arg & xmax) const {
      return m_a->integral(xmin, xmax) + m_b->integral(xmin, xmax);
   }

   virtual Function * clone() const {
      return new SumFunction(*this);
   }

protected:

   double value(const Arg & x) const {
      return m_a->operator()(x) + m_b->operator()(x);
   }

   void fetchDerivs(const Arg & x, std::vector<double> & derivs,
                    bool getFree) const;

};

} // namespace optimizers

#endif // optimizers_SumFunction_h
