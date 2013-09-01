/** 
 * @file Optimizer.h
 * @brief Declaration of Optimizer base class
 *
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Optimizer_h
#define optimizers_Optimizer_h

#include <iostream>

//#define HAVE_OPT_PP

namespace optimizers {

/** 
 * @class Optimizer
 *
 * @brief Abstract base class for objective function optimizers.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class Optimizer {
    
public:
    
   Optimizer() {}
   virtual ~Optimizer() {}

   virtual void find_min(int verbose, double tol) = 0;
    
};

} // namespace optimizers

#endif // optimizers_Optimizer_h
