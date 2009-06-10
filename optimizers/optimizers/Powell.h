/**
 * @class Powell
 * @file Powell.h
 * @brief Declaration for the Powell Optimizer subclass.
 * @author P. Nolan
 *
 * $Header:

An implementation of Powell's algorithm for minimization.
Powell doesn't use derivatives of the function, so this may be
useful in situations where derivatives are hard to calculate.

Warning: This does not implement bounds for the parameters.
If a search ventures into non-phyisical realms of parameter space,
there might be fatal math errors.

 */

#ifndef POWELL_H
#define POWELL_H

#include <vector>
#include "optimizers/Optimizer.h"
#include "optimizers/Statistic.h"

namespace optimizers {

  class Powell : public Optimizer {
  public:
    Powell(Statistic &stat) : Optimizer(stat) {}
    virtual ~Powell() {}
    virtual int find_min(int verbose=0, double tol=1e-8, int tolType=ABSOLUTE);
    virtual int find_min_only(int verbose=0, double tol=1e-8, int tolType=ABSOLUTE);
    double f1dim(double x);    
    void set_pcom(const std::vector<double> &pc) {m_pcom = pc;}
    void set_xicom(const std::vector<double> &xi) {m_xicom = xi;}
    virtual std::ostream& put(std::ostream&) const;
    double value(std::vector<double> &pval);
  private:
    std::vector<double> m_pcom;
    std::vector<double> m_xicom;
    int m_iter;
    double m_fret;
  };

  //! Functions lifted from Numerical Recipes, although with some modifications
  void mnbrak(double &ax, double &bx, double &cx, double &fa, double &fb, 
	      double &fc, Powell &func);

  int powell(std::vector<double> &p, std::vector<std::vector<double> > &xi, 
	      const double ftol, int tolType, int &iter, int maxit, double &fret, 
	      Powell &func);
  
  double brent(const double ax, const double bx, const double cx, 
	       Powell &f,const double tol, double &xmin);

  void linmin(std::vector<double> &p, std::vector<double> &xi, 
	      double &fret, Powell &func);

} // namespace 
#endif
