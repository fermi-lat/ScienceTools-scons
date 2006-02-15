/**
 * @file newMinuit.cxx
 * @brief Implementation of the newMinuit class
 * @author P. Nolan
 * $Header:
 */

#ifdef HAVE_NEWMINUIT

#include "optimizers/newMinuit.h"
#include "optimizers/Parameter.h"
#include "Minuit/MnUserParameters.h"
#include "Minuit/MnMigrad.h"
#include "Minuit/MnUserCovariance.h"
#include "Minuit/FunctionMinimum.h"
#include "Minuit/MnHesse.h"
#include "Minuit/MnPrint.h"
#include "Minuit/MinimumError.h"
#include "optimizers/Exception.h"
#include "optimizers/OutOfBounds.h"

namespace optimizers {

  typedef std::vector<Parameter>::iterator pptr;

  // Constructor
  newMinuit::newMinuit(Statistic & stat) : Optimizer(stat), m_maxEval(200), 
                                           m_fitDone(false), m_FCN(stat) { }

  // Call Minuit's MIGRAD to find the minimum of the function
  void newMinuit::find_min(int verbose, double tol, int TolType) {
    double tolerance = 2000. * tol;
    if (TolType == RELATIVE) tolerance *= fabs(m_stat->value());
    std::vector<Parameter> params;
    m_stat->getFreeParams(params);
    MnUserParameters upar;
    for (pptr p = params.begin(); p != params.end(); p++) {
      upar.add(p->getName().c_str(), p->getValue(), 1.0, 
	       p->getBounds().first, p->getBounds().second); 
      //  Q:  Is 1.0 the best choice for that parameter?
    }

    m_userState = MnUserParameterState(upar);
    MnMigrad migrad(m_FCN, m_userState, m_strategy);
    FunctionMinimum min = migrad(m_maxEval, tolerance);
    if (verbose > 0) std::cout << min;
    if (!min.isValid()) {
      throw Exception("Minuit abnormal termination.  No convergence?");
    }
    m_fitDone = true;
    m_userState = migrad.state();
    m_distance = min.edm();
    std::vector<double> ParamValues;
    unsigned int i = 0;
    for (pptr p = params.begin(); p != params.end(); p++, i++) {
      ParamValues.push_back(m_userState.parameters().value(i));
    }
    m_stat->setFreeParamValues(ParamValues);
  }

  // Call Minuit's HESSE to get a robust estimate of the covariance matrix
  void newMinuit::hesse(int verbose) {
    if (!m_fitDone) 
      throw Exception("Minuit: find_min must be executed before hesse");
    MnHesse hesse(m_strategy);
    m_userState = hesse(m_FCN, m_userState.parameters(), m_maxEval);
    if (verbose > 0) std::cout << m_userState;
    if (!m_userState.hasCovariance())
      throw Exception("Minuit HESSE results invalid");
  }

  // Constructor for the function to be minimized
  myFCN::myFCN(Statistic & stat): m_stat(&stat) {};

  // This is the function that Minuit minimizes
  double 
  myFCN::operator() (const std::vector<double> & params) const {
    try {m_stat->setFreeParamValues(params);}
    catch (OutOfBounds & e) {
      std::cerr << e.what() << std::endl;
      std::cerr<< "Value " << e.value() << " is not between"
	       << e.minValue() << " and " << e.maxValue() << std::endl;
      exit(e.code());
    }
    catch (Exception & e) {
      std::cerr << e.what() << std::endl;
      exit(e.code());
    }
    return -m_stat->value();
  }

  // Return values of the function gradient in the form
  // the Minuit wants
  std::vector<double> 
  myFCN::gradient(const std::vector<double> & params) const {
    try {m_stat->setFreeParamValues(params);}
    catch (OutOfBounds & e) {
      std::cerr << e.what() << std::endl;
      std::cerr<< "Value " << e.value() << " is not between"
	       << e.minValue() << " and " << e.maxValue() << std::endl;
      exit(e.code());
    }
    catch (Exception & e) {
      std::cerr << e.what() << std::endl;
      exit(e.code());
    }
    std::vector<double> grad;
    m_stat->getFreeDerivs(grad);
    for (unsigned int i = 0; i < grad.size(); i++) {
      grad[i] = -grad[i];
    }
    return grad;
  }

  // Get the uncertainty values from covariance matrix
  const std::vector<double> & newMinuit::getUncertainty(bool useBase) {
    if (useBase) {
      Optimizer::getUncertainty(useBase);
    } else {
      m_uncertainty.clear();
      for (unsigned int i = 0; i < m_userState.params().size(); i++) {
	m_uncertainty.push_back(sqrt(m_userState.covariance()(i,i)));
      }
    }
    return m_uncertainty;
  }

}
#endif
