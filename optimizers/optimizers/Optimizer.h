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

#include <vector>
#include <valarray>
#include <iostream>

namespace optimizers {

enum TOLTYPE {RELATIVE, ABSOLUTE};

class Statistic;

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
    
   Optimizer(Statistic &stat) {m_stat = &stat;}

   virtual ~Optimizer() {}

   virtual void find_min(int verbose, double tol, int tolType = RELATIVE) = 0;

   /// Returns the one-sigma confidence regions based on the Hessian,
   /// assuming that the statistic is a log-likelihood.
   virtual const std::vector<double> & getUncertainty(void);

protected:

   Statistic *m_stat;

   /// A vector to contain the estimated uncertainties of the free 
   /// parameters.
   std::vector<double> m_uncertainty;

   /// @param hess The Hessian matrix for the free parameters.
   /// @param eps The fractional step size used for computing the
   ///        finite difference approximations to the partial second 
   ///        derivatives.
   void computeHessian(std::valarray<double> &hess, double eps = 1e-5);

   /// @param hess Any symmetric, positive-definite square matrix.
   ///        On return, this matrix is replaced by the Cholesky 
   ///        decomposition and made fully symmetric.
   void choleskyDecompose(std::valarray<double> &hess);
   
};

/// Fortran routines
extern "C" {
  void drmngb_(const double * bounds, double * scale, double * funcval,
      	 double * grad, int * iv, const int * liv, const int *lv, 
      	 const int * n, double * v, double *x);
  void divset_(const int * kind, int * iv, const int * liv, const int * lv, 
      	double * v);
  int dpptri_(const char * uplo, const int * n, double * array,
      	 int * info, int strlen);
}

} // namespace optimizers

#endif // optimizers_Optimizer_h
