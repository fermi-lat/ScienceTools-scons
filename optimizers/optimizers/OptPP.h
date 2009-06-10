/** 
 * @file OptPP.h
 * @brief OptPP declaration
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_OptPP_h
#define optimizers_OptPP_h

#include "optimizers/Optimizer.h"
#include "optimizers/Statistic.h"

#ifdef HAVE_OPT_PP
#include "Opt.h"
#endif

namespace optimizers {

/** 
 * @class OptPP
 *
 * @brief Wrapper class for the OPT++ package
 * (http://csmr.ca.sandia.gov/projects/opt/).  Presently, we use their
 * bound constrained quasi-Newton optimizer OptBCQNewton with a
 * LineSearch strategy.  A more general interface that allows other
 * optimizer methods to be chosen will be implemented...eventually.
 *
 * @author J. Chiang
 *    
 * $Header$
 */

class OptPP : public Optimizer {
    
public:
    
   OptPP(Statistic &stat) : Optimizer(stat) {s_stat = &stat;}
   virtual ~OptPP() {}

   virtual int find_min(int verbose = 0, double tol = 1e-5, int tolType=0);
   virtual int find_min_only(int verbose = 0, double tol = 1e-5, int tolType=0);
   virtual std::ostream& put (std::ostream& s) const;

protected:

   static int s_verbose;

#ifdef HAVE_OPT_PP
   /// interface to the objective function that OPT++ expects
   static void statInterface(int mode, int ndim, const ColumnVector &x,
                             double &fx, ColumnVector &gx, int &result);

   /// returns the initial parameter values to the OPT++ routines
   static void statInit(int ndim, ColumnVector &x);

   /// do-nothing helper function for use with OptBCQNewton
   static void update_model(int, int, ColumnVector) {}
#endif

   static Statistic *s_stat;

};

} // namespace optimizers

#endif // optimizers_OptPP_h
