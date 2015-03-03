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

   Gaussian(double Prefactor=1, double Mean=150, double Sigma=15);

   virtual double integral(Arg & xmin, Arg & xmax) const;

   virtual Function * clone() const {
      return new Gaussian(*this);
   }

   const std::vector<double> & xvalues(size_t nx=100) const;

protected:

   double value(Arg &) const;

   double derivByParamImp(Arg &, const std::string & paramName) const;

private:

   double erfcc(double x) const;

   mutable std::vector<double> m_xvalues;

};

} // namespace optimizers

#endif // optimizers_Gaussian_h
