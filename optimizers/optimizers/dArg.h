/** 
 * @file dArg.h
 * @brief Declaration of dArg class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_dArg_h
#define optimizers_dArg_h

#include "optimizers/Arg.h"

namespace optimizers {

/** 
 * @class dArg
 *
 * @brief Concrete Arg subclass for encapsulating data of type double.
 *
 */

class dArg : public Arg {
    
public:
   
   dArg(double x) : m_val(x) {}
   virtual ~dArg() {}

   double getValue() const {
      return m_val;
   }

private:

   double m_val;

};

} // namespace optimizers

#endif // optimizers_dArg_h
