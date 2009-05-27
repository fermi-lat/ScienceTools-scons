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

#include "optimizers/Exception.h"
#include "optimizers/Statistic.h"

namespace optimizers {

enum TOLTYPE {RELATIVE, ABSOLUTE};

// class Statistic;

/** 
 * @class Optimizer
 *
 * @brief Abstract base class for objective function optimizers.
 *
 */

class Optimizer {
    
public:
    
   Optimizer(Statistic & stat) : m_stat(&stat), m_maxEval(100*m_stat->getNumParams()) {}

   virtual ~Optimizer() {}

   virtual void find_min(int verbose, double tol, int tolType = ABSOLUTE) = 0;
   virtual void find_min_only(int verbose, double tol, int tolType = ABSOLUTE) = 0;

   /// Returns the one-sigma confidence regions based on the Hessian,
   /// assuming that the statistic is a log-likelihood.
   virtual const std::vector<double> & getUncertainty(bool useBase=false);

   /// Returns the covariance matrix
   virtual std::vector<std::vector<double> > covarianceMatrix() const;

   /// MINOS error analysis for parameter #n.  Valid only for the
   /// two flavors of Minuit.
   virtual std::pair<double,double> Minos(unsigned int) {
     throw Exception("Minos function is not enabled for this optimizer");
     return std::pair<double,double>(0., 0.);
   }

   Statistic & stat() {
      return *m_stat;
   }

   const Statistic & stat() const {
      return *m_stat;
   }

   void setMaxEval(const int maxEval) {m_maxEval = maxEval;}
   int  getMaxEval() const {return m_maxEval;}

   virtual std::ostream& put (std::ostream& s) const = 0;

protected:

   Statistic * m_stat;

   int m_maxEval;

   /// A vector to contain the estimated uncertainties of the free 
   /// parameters.
   std::vector<double> m_uncertainty;

//    /// A matrix to contain the covariance matrix of the free parameters
//    std::vector<std::vector<double> > m_covariancematrix;

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

std::ostream& operator<<(std::ostream& s, const Optimizer& t);

/// Fortran routines
extern "C" {
  void drmngb_(const double * bounds, double * scale, double * funcval,
      	 double * grad, int * iv, const int * liv, const int *lv, 
      	 const int * n, double * v, double *x);
  void drmnfb_(const double * bounds, double * scale, double * funcval,
      	 int * iv, const int * liv, const int *lv, 
      	 const int * n, double * v, double *x);
  void divset_(const int * kind, int * iv, const int * liv, const int * lv, 
      	double * v);
  int dpptri_(const char * uplo, const int * n, double * array,
      	 int * info, int strlen);
}

} // namespace optimizers

#endif // optimizers_Optimizer_h
