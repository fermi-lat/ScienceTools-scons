/**
 * @file NewMinuit.h
 * @brief Interface to the new C++ version of Minuit
 * @author P. Nolan
 * $Header$
 */

#ifndef optimizers_NEWMINUIT_H
#define optimizers_NEWMINUIT_H

#include <vector>
#include "optimizers/Optimizer.h"
#include "optimizers/Statistic.h"
#include "Minuit2/FCNGradientBase.h"
#include "Minuit2/MnStrategy.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/FunctionMinimum.h"

namespace optimizers {

  /**
   * @class myFCN
   * @brief The function minimized by Minuit
   * @author P. Nolan
   Minuit provides a base class for the function to be minimized.
   This is the implementation using the Optimizer infrastructure.

   Q:  Would it be better to do this as a hidden class within NewMinuit?
  */

  class myFCN : public ROOT::Minuit2::FCNGradientBase {
  public:
    myFCN(Statistic &);
    virtual ~myFCN() {};
    virtual double Up() const {return m_level;}
    virtual double operator() (const std::vector<double> &) const;
    virtual std::vector<double> Gradient(const std::vector<double> &) const;
    virtual bool CheckGradient() const {return false;}
    virtual void SetErrorDef(double level) {m_level=level;}
  private:
    Statistic * m_stat;
    double m_level;
  };

  /**
   * @class NewMinuit
   * @brief Wrapper class for the Minuit optimizer from CERN
   * @author P. Nolan
   This class implements an Optimizer by using Minuit, a well-known
   package from CERN.  It uses only a few of Minuit's features.
   It uses only the Migrad and Hesse algorithms.  All variables are
   treated as bounded.  No user interaction is allowed.  The new
   C++ implementation of Minuit is used, which has no limits on the
   number of free parameters.  The older Fortran version of Minuit is
   well known in the HEP community.  It was developed at CERN over a
   span of about 30 years.
  */

  class NewMinuit : public Optimizer {
  public:
    NewMinuit(Statistic &);
    virtual ~NewMinuit() {delete m_min;};
    NewMinuit & operator=(const NewMinuit & rhs);
    NewMinuit(const NewMinuit & x);
    virtual int find_min(int verbose=0, double tole = 1e-5, int tolType = ABSOLUTE);
    virtual int find_min_only(int verbose=0, double tole = 1e-5, int tolType = ABSOLUTE);
    void setStrategy(unsigned int strat = 1) {
       m_strategy_value = strat;
       m_strategy=ROOT::Minuit2::MnStrategy(strat);
    }
     unsigned int getStrategy() const {
        return m_strategy_value;
     }

    double getDistance(void) const {return m_distance;};
    virtual const std::vector<double> & getUncertainty(bool useBase = false);
    virtual std::vector<std::vector<double> > covarianceMatrix() const;
    virtual std::ostream& put (std::ostream& s) const;
    std::pair<double,double> Minos(unsigned int n, double level=1.);

     /// Compute the lower bound error using Minos.
     double minos_lower_error(unsigned int n, double level=1.,
                              double tol=1e-3);
     /// Compute the upper bound error using Minos.
     double minos_upper_error(unsigned int n, double level=1.,
                              double tol=1e-3);

    /// Run a MNCONTOUR dynamic CONTOUR analysis
    void MnContour(unsigned int par1, unsigned int par2,
		   double level=1., unsigned int npts=20);
  private:
    myFCN m_FCN;
    double m_distance;
    double m_tolerance;
    ROOT::Minuit2::MnStrategy m_strategy;
    ROOT::Minuit2::FunctionMinimum * m_min;

     unsigned int m_strategy_value;

    void setTolerance(double tol, int tolType);
    void hesse(int verbose = 0);
    int checkResults();

     void checkParValues(unsigned int n, std::vector<double> & parValues) const;
  };

}
#endif
