/** 
 * @file Gaussian.h
 * @brief Gaussian class declaration
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Gaussian_h
#define optimizers_Gaussian_h

#include "optimizers/Function.h"
#include "optimizers/Arg.h"

namespace optimizers {
/** 
 * @class Gaussian
 *
 * @brief A 1D Gaussian function
 *
 * @author J. Chiang
 *    
 * $Header$
 */
    
class Gaussian : public Function {
public:

   Gaussian(){init(0, 0, 1);}
   Gaussian(double Prefactor, double Mean, double Sigma)
      {init(Prefactor, Mean, Sigma);}

   double value(Arg &) const;

   double derivByParam(Arg &, const std::string &paramName) const
      throw(ParameterNotFound);

   double integral(Arg &xmin, Arg &xmax) const;

   virtual Function *clone() const {
      return new Gaussian(*this);
   }

private:

   void init(double Prefactor, double Mean, double Sigma);
   double erfcc(double x) const;

};

} // namespace optimizers

#endif // optimizers_Gaussian_h
