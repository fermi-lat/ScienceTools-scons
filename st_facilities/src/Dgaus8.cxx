/**
 * @file Dgaus8.cxx
 * @brief Class wrapper for dgaus8
 * @author J. Chiang
 *
 * $Header$
 */

#include <vector>

#include "st_facilities/dgaus8.h"
#include "st_facilities/Dgaus8.h"

namespace st_facilities {

double Dgaus8::integrate(D_fp func, double xmin, double xmax,
                         double error, long & ier) {
   double integral(0);
   dgaus8_(func, &xmin, &xmax, &error, &integral, &ier);
   if (ier == 1) {
      return integral;
   }

   size_t nx(1000);
   double xstep((xmax - xmin)/(nx - 1));
   std::vector<double> xx, yy;
   integral = 0;
   for (size_t i(0); i < nx; i++) {
      xx.push_back(xstep*i + xmin);
      yy.push_back(func(&xx.back()));
   }
   for (size_t i(0); i < nx-1; i++) {
      integral += (yy.at(i+1) + yy.at(i))/2.*(xx.at(i+1) - xx.at(i));
   }
   return integral;
}

} // namespace st_facilities

