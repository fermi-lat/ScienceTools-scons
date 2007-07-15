/** 
 * @file Amoeba.cxx
 * @brief Use Nelder-Mead to minimize a function object.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <iostream>
#include <stdexcept>

#include "optimizers/Amoeba.h"

namespace {

void print_p(const std::vector< std::vector<double> > & p, 
             const std::vector<double> & yvals) {
   for (size_t i(0); i < p.size(); i++) {
      for (size_t j(0); j < p.at(i).size(); j++) {
//         std::cout << p.at(i).at(j) << "  ";
         }
//      std::cout << yvals.at(i) << ";  ";
   }
//   std::cout << std::endl;
}

double amotry(std::vector< std::vector<double> > & p,
              std::vector<double> & y,
              std::vector<double> & psum,
              int ndim, optimizers::Functor & func, int ihi, double fac) {
   std::vector<double> ptry(ndim, 0);
   double fac1((1. - fac)/float(ndim));
   double fac2(fac1 - fac);
   for (int j = 0; j < ndim; j++) {
      ptry.at(j) = psum.at(j)*fac1 - p.at(ihi).at(j)*fac2;
   }
   double ytry(func(ptry));
   if (ytry < y.at(ihi)) {
      y.at(ihi) = ytry;
      for (int j = 0; j < ndim; j++) {
         psum.at(j) += ptry.at(j) - p.at(ihi).at(j);
         p.at(ihi).at(j) = ptry.at(j);
      }
   }
   print_p(p, y);
   return ytry;
}

void swap(double & a, double & b) {
   double tmp(a);
   a = b;
   b = tmp;
}

void amoeba(std::vector< std::vector<double> > & p,
            std::vector<double> & y, int ndim, double ftol,
            optimizers::Functor & func, int & nfunc) {
   static int nmax(5000);
   int mpts(ndim + 1);
   std::vector<double> psum(ndim, 0);
   nfunc = 0;
   for (int j = 0; j < ndim; j++) {
      double sum(0);
      for (int i = 0; i < mpts; i++) {
         sum += p.at(i).at(j);
      }
      psum.at(j) = sum;
   }
   while(true) {
      int ilo(0);
      int inhi;
      int ihi = y.at(0) > y.at(1) ? (inhi=1,0) : (inhi=0,1);
      for (int i = 0; i < mpts; i++) {
         if (y.at(i) <= y.at(ilo)) {
            ilo = i;
         }
         if (y.at(i) > y.at(ihi)) {
            inhi = ihi;
            ihi = i;
         } else if (y.at(i) > y.at(inhi) && i != ihi) {
            inhi = i;
         }
      }
      double num(2.0*std::fabs(y.at(ihi) - y.at(ilo)));
      double denom(std::fabs(y.at(ihi)) + std::fabs(y.at(ilo)));
      double rtol;
//       std::cout << "num = " << num << "  "
//                 << "denom = " << denom << "  ";
      if (denom == 0) {  /// treat the tolerance as absolute
         rtol = num;
      } else {
         rtol = num/denom;
      }
//      std::cout << "rtol = " << rtol << std::endl;
      if (rtol < ftol) {
         swap(y.at(0), y.at(ilo));
         for (int i = 0; i < ndim; i++) {
            swap(p.at(0).at(i), p.at(ilo).at(i));
         }
         break;
      }
      if (nfunc >= nmax) {
         throw std::runtime_error("amoeba: nmax exceeded");
      }
      nfunc += 2;
      double ytry(amotry(p, y, psum, ndim, func, ihi, -1.0));
      if (ytry <= y.at(ilo)) {
         ytry = amotry(p, y, psum, ndim, func, ihi, 2.0);
      } else if (ytry >= y.at(inhi)) {
         double ysave(y.at(ihi));
         ytry = amotry(p, y, psum, ndim, func, ihi, 0.5);
         if (ytry >= ysave) {
            for (int i = 0; i < mpts; i++) {
               if (i != ilo) {
                  for (int j = 0; j < ndim; j++) {
                     p.at(i).at(j) = psum.at(j) = 
                        0.5*(p.at(i).at(j) + p.at(ilo).at(j));
                  }
                  y.at(i) = func(psum);
               }
            }
            print_p(p, y);
            nfunc += ndim;
            for (int j = 0; j < ndim; j++) {
               double sum(0);
               for (int i = 0; i < mpts; i++) {
                  sum += p.at(i).at(j);
               }
               psum.at(j) = sum;
            }
         } else {
            --nfunc;
         }
      }
   }
}

} // anonymous namespace

namespace optimizers {

double Amoeba::findMin(std::vector<double> & params, double tol) {
   std::vector<double> yvalues;
   yvalues.reserve(m_npars + 1);
   for (size_t i = 0; i < m_npars + 1; i++) {
      double yval(m_functor(m_simplex.at(i)));
      yvalues.push_back(yval);
   }
   int nevals(0);
   ::amoeba(m_simplex, yvalues, m_npars, tol, m_functor, nevals);
   int imin(0);
   double ymin(yvalues.at(imin));
   for (size_t i = 1; i < m_npars + 1; i++) {
      if (yvalues.at(i) < ymin) {
         imin = i;
         ymin = yvalues.at(imin);
      }
   }
   params = m_simplex.at(imin);
   return ymin;
}

void Amoeba::buildSimplex(const std::vector<double> & params,
                          double frac, bool addfrac) {
   m_simplex.clear();
   m_simplex.reserve(m_npars);
   m_simplex.push_back(params);
   for (size_t i = 1; i < m_npars+1; i++) {
      m_simplex.push_back(params);
      double & value(m_simplex.back()[i-1]);
      if (addfrac) {
         value += frac;
      } else {
         if (value == 0) { // moderately fragile kluge for this special case
            value = frac;
         } else {
            value *= (1. + frac);
         }
      }
   }
}

} // namespace optimizers
