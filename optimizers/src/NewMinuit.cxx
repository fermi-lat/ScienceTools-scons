/**
 * @file NewMinuit.cxx
 * @brief Implementation of the NewMinuit class
 * @author P. Nolan
 * $Header$
 */

#include "optimizers/NewMinuit.h"
#include "optimizers/Parameter.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnHesse.h"
#include "Minuit2/MnPrint.h"
#include "optimizers/Exception.h"
#include "optimizers/OutOfBounds.h"

namespace optimizers {

  typedef std::vector<Parameter>::iterator pptr;

  // Constructor
  NewMinuit::NewMinuit(Statistic & stat) 
  : Optimizer(stat), m_maxEval(0), m_fitDone(false), m_FCN(stat), 
    m_strategy(ROOT::Minuit2::MnStrategy(1)) {}

  // Call Minuit's MIGRAD to find the minimum of the function
  void NewMinuit::find_min(int verbose, double tol, int TolType) {
    double tolerance = 1000. * tol;
    if (TolType == RELATIVE) tolerance *= fabs(m_stat->value());
    std::vector<Parameter> params;
    m_stat->getFreeParams(params);
    ROOT::Minuit2::MnUserParameters upar;
    size_t ii(0);
    for (pptr p = params.begin(); p != params.end(); p++, ii++) {
       std::ostringstream mangledName;
       mangledName << ii << "_" << p->getName();
       upar.Add(mangledName.str().c_str(), p->getValue(), 1.0, 
	       p->getBounds().first, p->getBounds().second); 
      //  Q:  Is 1.0 the best choice for that parameter?
    }

    m_userState = ROOT::Minuit2::MnUserParameterState(upar);
    ROOT::Minuit2::MnMigrad migrad(m_FCN, m_userState, m_strategy);
    ROOT::Minuit2::FunctionMinimum min = migrad(m_maxEval, tolerance);
    m_min = new ROOT::Minuit2::FunctionMinimum(min);
    if (verbose > 0) std::cout << m_min;
    if (!min.IsValid()) {
      throw Exception("Minuit abnormal termination.  No convergence?");
    }
    m_fitDone = true;
    m_userState = migrad.State();
    m_distance = min.Edm();
    std::vector<double> ParamValues;
    unsigned int i = 0;
    for (pptr p = params.begin(); p != params.end(); p++, i++) {
      ParamValues.push_back(m_userState.Parameters().Value(i));
    }
    m_stat->setFreeParamValues(ParamValues);
  }

  // Call Minuit's HESSE to get a robust estimate of the covariance matrix
  void NewMinuit::hesse(int verbose) {
    if (!m_fitDone) 
      throw Exception("Minuit: find_min must be executed before hesse");
    ROOT::Minuit2::MnHesse hesse(m_strategy);
    m_userState = hesse(m_FCN, m_userState.Parameters(), m_maxEval);
    if (verbose > 0) std::cout << m_userState;
    if (!m_userState.HasCovariance())
      throw Exception("Minuit HESSE results invalid");
  }

  // Call MINOS
  std::pair<double,double> NewMinuit::Minos(unsigned int n) {
    int npar = m_userState.Params().size();
    if (n < 0 || n >= npar) {
      throw Exception("Parameter number out of range in Minos", n);
    }
    ROOT::Minuit2::MnMinos mns(m_FCN, *m_min, m_strategy);
    return mns(n);
  }

  // Constructor for the function to be minimized
  myFCN::myFCN(Statistic & stat): m_stat(&stat) {}

  // This is the function that Minuit minimizes
  double 
  myFCN::operator() (const std::vector<double> & params) const {
    try {m_stat->setFreeParamValues(params);}
    catch (OutOfBounds & e) {
      std::cerr << e.what() << std::endl;
      std::cerr << "Value " << e.value() << " is not between"
                << e.minValue() << " and " << e.maxValue() << std::endl;
      throw;
    }
    catch (Exception & e) {
      std::cerr << e.what() << std::endl;
      throw;
    }
    return -m_stat->value();
  }

  // Return values of the function gradient in the form
  // that Minuit wants
  std::vector<double> 
  myFCN::Gradient(const std::vector<double> & params) const {
    try {m_stat->setFreeParamValues(params);}
    catch (OutOfBounds & e) {
      std::cerr << e.what() << std::endl;
      std::cerr<< "Value " << e.value() << " is not between"
	       << e.minValue() << " and " << e.maxValue() << std::endl;
      throw;
    }
    catch (Exception & e) {
      std::cerr << e.what() << std::endl;
      throw;
    }
    std::vector<double> grad;
    m_stat->getFreeDerivs(grad);
    for (unsigned int i = 0; i < grad.size(); i++) {
      grad[i] = -grad[i];
    }
    return grad;
  }

  // Get the uncertainty values from covariance matrix
   const std::vector<double> & NewMinuit::getUncertainty(bool useBase) {
      if (useBase) {
         Optimizer::getUncertainty(useBase);
      } else {
         if (!m_userState.HasCovariance()) {
            hesse(0);
         }
         m_uncertainty.clear();
         for (size_t i = 0; i < m_userState.Params().size(); i++) {
            m_uncertainty.push_back(sqrt(m_userState.Covariance()(i, i)));
         }
      }
      return m_uncertainty;
   }

  std::ostream& NewMinuit::put (std::ostream& s) const {
    s << m_userState;
    return s;
  }

   std::vector<std::vector<double> > NewMinuit::covarianceMatrix() const {
      if (!m_userState.HasCovariance()) {
         const_cast<NewMinuit *>(this)->hesse(0);
      }
      std::vector<std::vector<double> > covariancematrix;
      
      ROOT::Minuit2::MnUserCovariance cov = m_userState.Covariance();
      
      for (unsigned int x = 0; x < cov.Nrow(); ++x) {
         std::vector<double> vec;
         for (unsigned int y = 0; y < cov.Nrow(); ++y) {
            vec.push_back(cov(x,y));
         }
         covariancematrix.push_back(vec);
      }
      
      return covariancematrix;
   }
      
} // namespace optimizers
