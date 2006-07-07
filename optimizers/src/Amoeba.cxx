/** 
 * @file Amoeba.cxx
 * @brief Use Nelder-Mead to minimize a function object.
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>

#include "optimizers/Amoeba.h"

namespace optimizers {

Amoeba * Amoeba::s_instance = 0;
Functor * Amoeba::s_functor = 0;
std::vector<std::vector<double> > Amoeba::s_startingSimplex;
int Amoeba::s_npars;

extern "C" {
   void amoeba(double **p, double y[], int ndim, double ftol, 
               double (*funk)(double *), int *nfunk);
}

double Amoeba::findMin(std::vector<double> & params, double tol) {
   std::vector<double> yvalues;
   yvalues.reserve(s_npars + 1);
   for (int i = 0; i < s_npars + 1; i++) {
      double yval = (*s_functor)(s_startingSimplex[i]);
      yvalues.push_back(yval);
   }
   int nevals;
   std::vector<double*> my_simplex;
   my_simplex.reserve(s_npars + 1);
   for (int i = 0; i < s_npars + 1; i++) {
      my_simplex.push_back(&s_startingSimplex[i][0]-1);
   }
   amoeba(&my_simplex[0]-1, &yvalues[0]-1, s_npars, tol, &func, &nevals);
   int imin = 0;
   double ymin = yvalues[imin];
   for (int i = 1; i < s_npars + 1; i++) {
      if (yvalues[imin] < ymin) {
         imin = i;
         ymin = yvalues[imin];
      }
   }
   params = s_startingSimplex[imin];
   return ymin;
}

void Amoeba::buildStartingSimplex(const std::vector<double> & params,
                                  std::vector<std::vector<double> > & simplex,
                                  double frac, bool addfrac) {
   int npars = params.size();
   simplex.clear();
   simplex.reserve(npars);
   simplex.push_back(params);
   for (int i = 1; i < npars+1; i++) {
      simplex.push_back(params);
      double & value = simplex.back()[i-1];
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
