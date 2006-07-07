/**
 * @file Amoeba.h
 * @brief Class to minimize a functor using Nelder-Mead.
 * @author J. Chiang <jchiang@slac.stanford.edu>
 *
 * $Header$
 */

#ifndef optimizers_Amoeba_h
#define optimizers_Amoeba_h

#include <iostream>
#include <stdexcept>
#include <string>

#include "optimizers/Functor.h"

namespace optimizers {

/**
 * @class Amoeba
 *
 * @brief This class minimizes a function object that takes
 * std::vector<double> of parameter values using the Nelder-Mead
 * algorithm implemented in NR.
 *
 * Since the functor is stored as static data member, which is
 * necessary to interface to the NR amoeba routine, only one instance
 * of this object can be created, hence it is itself Singleton.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Amoeba {

public:

   static Amoeba * 
   instance(Functor &functor, 
            std::vector<std::vector<double> > & startingSimplex) {
      if (s_instance == 0) {
         s_instance = new Amoeba(functor, startingSimplex);
      } else {
         throw std::invalid_argument("A functor already exists. "
                                     + std::string("Delete the Singleton ")
                                     + "instance first before attempting to "
                                     + std::string("minimize a different ")
                                     + "function object.");
      }
      return s_instance;
   }

   static Amoeba * instance() {
      return s_instance;
   }

   ~Amoeba() {}

   static void delete_instance() {
      delete s_instance;
      s_instance = 0;
   }

   /// @return The estimate of the minimum value of the functor.
   /// @param params The parameter values at the minimum estimate.
   /// @param tol The absolute tolerance for convergence.
   double findMin(std::vector<double> & params, double tol = 1e-15);

   /// @param params An ndim vector of parameter values as a candidate
   /// starting point for the minimization.
   /// @param simplex The ndim+1 by ndim vector as the starting simplex
   /// constructed by looping through each dimension and setting the
   /// parameter value to (1+frac) times the input value.
   /// @param frac This sets the size of the initial simplex.
   static void buildStartingSimplex(const std::vector<double> & params,
                                    std::vector<std::vector<double> > &simplex,
                                    double frac = 0.1, bool addfrac = false);

protected:

   /// @param functor The function object to be minimized.  It must
   /// take as an argument a std::vector<double>, representing the
   /// parameter values at which it is to be evaluated.
   /// @param startingSimplex Effectively an ndim+1 by ndim array
   /// containing the vertices of the starting simplex.
   Amoeba(Functor &functor,
          std::vector<std::vector<double> > & startingSimplex) {
      s_functor = &functor;
      s_startingSimplex = startingSimplex;
      s_npars = startingSimplex.size() - 1;
   }

private:

   static Amoeba * s_instance;

   static Functor * s_functor;

   static std::vector<std::vector<double> > s_startingSimplex;
   
   static int s_npars;

   static double func(double *x) {
      std::vector<double> xvals;
      for (int i = 1; i < s_npars+1; i++) {
         xvals.push_back(x[i]);
      }
      return (*s_functor)(xvals);
   }

};

} // namespace optimizers

#endif // optimizers_Amoeba_h
