/**
 * @file Util.cxx
 * @brief Implementation for optimizer package utilities.
 * @author J. Chiang
 *
 * $Header$
 */

#include <cmath>

#include <algorithm>
#include <iostream>
#include <vector>

#include "optimizers/Util.h"

namespace {
   static double con(1.4);
   static double con2(con*con);
   static double big(1e30);
//   static size_t ntab(10);
   static size_t ntab(1);
   static double safe(2.);
}

namespace optimizers {

double Util::numDeriv(FunctorBase & f, double x, double h, double & err) {

   std::cout << "inside Util::numDeriv" << std::endl;

   std::vector< std::vector<double> > aa(::ntab);
   for (size_t j=0; j < ::ntab; j++) {
      aa.at(j).resize(::ntab, 0);
   }

   aa.at(0).at(0) = (f(x+h) - f(x-h))/2./h;

   err = big;

   double ans;
   for (size_t i=1; i < ::ntab; i++) {
      h /= ::con;
      aa.at(0).at(i) = (f(x+h) - f(x-h))/2./h;
      double fac(::con2);
      for (size_t j=1; j <= i; j++) {
         aa.at(j).at(i) = (aa.at(j-1).at(i)*fac - aa.at(j-1).at(i-1))/(fac-1.);
         fac *= ::con2;
         double errt(std::max(std::fabs(aa.at(j).at(i) - aa.at(j-1).at(i)),
                              std::fabs(aa.at(j).at(i) - aa.at(j-1).at(i-1))));
         if (errt <= err) {
            err = errt;
            ans = aa.at(j).at(i);
         }
      }
      if (std::fabs(aa.at(i).at(i) - aa.at(i-1).at(i-1)) >= ::safe*err) {
         return ans;
      }
   }
   return ans;
}

} // namespace optimizers
