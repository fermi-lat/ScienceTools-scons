/**
 * @file Mcmc.h
 * @brief Mcmc (Markov Chain Monte Carlo) class declaration
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Mcmc_h
#define optimizers_Mcmc_h

#include <vector>
#include <string>
#include "optimizers/Parameter.h"
#include "optimizers/Function.h"
#include "optimizers/Exception.h"

namespace optimizers {

/**
 * @class Mcmc
 *
 * @brief Apply the Variable-at-a-Time Metropolis-Hastings algorthim
 * to the (free) Parameters of a Function object.
 *
 * The transition probability distributions are specified along each
 * dimension by top-hat functions.  The widths of these top-hats may
 * either be estimated, by default, as the rough 1-sigma error using
 * an approximate Hessian at the starting point via e.g., Minuit; or
 * they may be specified by hand through the setTransitionWidths
 * method.  Because the Parameters are generally bounded, the
 * transition probabilities must be renormalized at each trial point
 * by the fraction of the top-hat contained within the boudaries,
 * hence the need for Metropolis-Hastings rather than simply the
 * Metropolis algorithm.
 *
 * Priors that are functions of the same set of Parameters in the form
 * of other Function objects can be applied.  As with the Statistic
 * itself, these priors need not be normalized to unity.
 *
 * @author J. Chiang
 *
 * $Header$
 */

class Mcmc {

public:

   Mcmc(Function &stat, bool verbose=true);

   ~Mcmc() {}

   void addPriors(std::vector<Function *> & priors) {
      m_priors = priors;
   }

   void generateSamples(std::vector< std::vector<double> > &samples,
                        unsigned long nsamp=1e4);

   /// Set the transition probablity widths by hand
   void setTransitionWidths(std::vector<double> &transitionWidths) {
      m_transitionWidths = transitionWidths;
   }

   /// Useful for restarting the MCMC
   void getTransitionWidths(std::vector<double> &transitionWidths) {
      transitionWidths = m_transitionWidths;
   }

   /// write samples to a FITS binary table
   void writeSamples(std::string filename, 
                     std::vector< std::vector<double> > &samples) const;

private:

   Function * m_stat;

   bool m_verbose;

   std::vector<Function *> m_priors;

   std::vector<double> m_transitionWidths;

   void estimateTransWidths();

   double drawValue(Parameter &param, double transitionWidth, 
                    double &transProbRatio);

};

} // namespace optimizers

#endif // optimizers_Mcmc_h
