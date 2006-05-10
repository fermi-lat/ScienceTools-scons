/**
 * @file Optimizer.cxx
 * @brief Implementation for the Optimizer base class.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <sstream>

#include "optimizers/dArg.h"
#include "optimizers/Exception.h"
#include "optimizers/Optimizer.h"
#include "optimizers/Statistic.h"
#include "optimizers/Util.h"

namespace {
   class StatDerivFunc {
   public:
      StatDerivFunc(optimizers::Statistic & stat, size_t ipar, size_t jpar) 
         : m_stat(stat), m_ipar(ipar), m_jpar(jpar) {
         m_stat.getFreeParamValues(m_parVals);
      }
      double operator()(double parVal) const {
         std::vector<double> parVals(m_parVals);
         parVals.at(m_jpar) = parVal;
         m_stat.setFreeParamValues(parVals);
         std::vector<double> derivs;
         m_stat.getFreeDerivs(derivs);
         return derivs.at(m_ipar);
      }
   private:
      optimizers::Statistic & m_stat;
      size_t m_ipar;
      size_t m_jpar;
      std::vector<double> m_parVals;
   };
}

namespace optimizers {

const std::vector<double> & Optimizer::getUncertainty(bool) {
   double eps(1e-7);
   std::valarray<double> hess;
   computeHessian(hess, eps);

   choleskyDecompose(hess);

// Repack in compact form.
   int npars = static_cast<int>(sqrt(double(hess.size()))+0.1);
   std::valarray<double> compactHess(npars*(npars+1)/2);
   int indx(0);
   for (int j = 0; j < npars; j++) {
      for (int i = 0; i <= j; i++) {
         compactHess[indx++] = hess[i + j*npars];
      }
   }

// Invert using the BLAS routine to get the covariance matrix.
   int info;
   const char uplo = 'U';
   dpptri_(&uplo, &npars, &compactHess[0], &info, 1);
   if (info < 0) {
      throw Exception("DPPTRI: illegal argument value", -info);
   } else if (info > 0) {
      throw Exception("DPPTRI: Zero diagonal element in Cholesky factor",
                      info);
   }

// Extract the error estimates as the square-roots of the diagonal
// elements.
   m_uncertainty.clear();
   for (int i = 0; i < npars; i++) {
      m_uncertainty.push_back(sqrt(compactHess[i*(i + 3)/2]));
   }

   return m_uncertainty;
}

#if 1
void Optimizer::computeHessian(std::valarray<double> &hess, double eps) {
// Compute the Hessian matrix for the free parameters using simple
// finite differences.
   std::vector<double> params;
   m_stat->getFreeParamValues(params);

   std::vector<double> firstDerivs;
   m_stat->getFreeDerivs(firstDerivs);

// Obtain the full Hessian matrix.
   int npars = params.size();
   hess.resize(npars*npars);
   int indx(0);
   for (int irow = 0; irow < npars; irow++) {
      std::vector<double> new_params = params;
      double delta;
      if (params[irow] == 0) {
         delta = eps;
      } else {
         delta = params[irow]*eps;
      }
      new_params[irow] = params[irow] + delta;
      m_stat->setFreeParamValues(new_params);
      std::vector<double> derivs;
      m_stat->getFreeDerivs(derivs);
      for (int icol = 0; icol < npars; icol++) {
         hess[indx] = -(derivs[icol] - firstDerivs[icol])/delta;
         indx++;
      }
   }
// Restore Parameter values.
   m_stat->setFreeParamValues(params);
}
#else
void Optimizer::computeHessian(std::valarray<double> & hess, double eps) {
   std::vector<double> params;
   m_stat->getFreeParamValues(params);
   size_t npars(params.size());
   hess.resize(npars*npars);
   size_t indx(0);
   for (size_t i=0; i < npars; i++) {
      for (size_t j=0; j < npars; j++, indx++) {
         ::StatDerivFunc statFunc(*m_stat, i, j);
         Functor< ::StatDerivFunc > my_func(statFunc);
         double err;
         double h(std::fabs(params.at(j)*eps));
         if (h == 0) {
            h = eps;
         }
         hess[indx] = -Util::numDeriv(my_func, params.at(j), h, err);
      }
   }
   m_stat->setFreeParamValues(params);
}
#endif

void Optimizer::choleskyDecompose(std::valarray<double> &array) {
// This implementation is based on NR's choldc().

   int npts = static_cast<int>(sqrt(double(array.size()))+0.1);
   std::valarray<double> p(npts);

   for (int i = 0; i < npts; i++) {
      for (int j = i; j < npts; j++) {
// Here we use the FORTRAN subscripting convention, 
// a[i][j] = array[i + j*npts].
         double sum = array[i + j*npts];
         for (int k = i - 1; k >= 0; k--) {
            sum -= array[i + k*npts]*array[j + k*npts];
         }
         if (i == j) {
            if (sum <= 0) {
               std::ostringstream errorMessage;
               errorMessage << "Optimizer::choleskyDecompose:\n"
                            << "Imaginary diagonal element.";
               throw Exception(errorMessage.str());
            }
            p[i] = sqrt(sum);
         } else {
            array[j + i*npts] = sum/p[i];
         }
      }
   }

// Symmetrize.
   for (int i = 0; i < npts; i++) {
      for (int j = i; j < npts; j++) {
         if (i == j) {
            array[i + j*npts] = p[i];
         } else {
            array[i + j*npts] = array[j + i*npts];
         }
      }
   }
}

} // namespace optimizers
