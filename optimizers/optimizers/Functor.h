/**
 * @file Functor.h
 * @brief Function object interface definition to be used by Amoeba.
 * @author J. Chiang
 * 
 * $Header$
 */

#ifndef optimizers_Functor_h
#define optimizers_Functor_h

#include <vector>

namespace optimizers {

/**
 * @class Functor
 * @brief All function objects to be minimized by Amoeba must derive 
 * from this class.
 * @author J. Chiang
 *
 * $Header$
 */

class Functor {

public:

   Functor() {}

   virtual ~Functor() {}

   virtual double operator()(std::vector<double> & x) = 0;

};

} // namespace optimizers

#endif // optimizers_Functor_h
