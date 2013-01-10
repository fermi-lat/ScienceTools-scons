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
 */
    
class Gaussian : public Function {
public:

   Gaussian() {
      init(1, 150, 15);
   }

   Gaussian(double Prefactor, double Mean, double Sigma) {
      init(Prefactor, Mean, Sigma);
   }

   double value(Arg &) const;

   double derivByParam(Arg &, const std::string &paramName) const;

   double integral(Arg &xmin, Arg &xmax) const;

   virtual Function *clone() const {
      return new Gaussian(*this);
   }

   const std::vector<double> & xvalues(size_t nx=100) const;

private:

   void init(double Prefactor, double Mean, double Sigma);
   double erfcc(double x) const;

};

} // namespace optimizers

#endif // optimizers_Gaussian_h
