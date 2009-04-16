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
#include "RVersion.h"

namespace optimizers {

  typedef std::vector<Parameter>::iterator pptr;

  // Constructor
  NewMinuit::NewMinuit(Statistic & stat) 
  : Optimizer(stat), m_maxEval(0),  m_FCN(stat), 
    m_tolerance(1e-3), m_strategy(ROOT::Minuit2::MnStrategy(1)), m_min(0) {}

  // Call Minuit's MIGRAD to find the minimum of the function
  void NewMinuit::find_min(int verbose, double tol, int TolType) {
    setTolerance(tol, TolType);
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

    ROOT::Minuit2::MnUserParameterState userState(upar);
    ROOT::Minuit2::MnMigrad migrad(m_FCN, userState, m_strategy);
    ROOT::Minuit2::FunctionMinimum min = migrad(m_maxEval, m_tolerance);
    m_min = new ROOT::Minuit2::FunctionMinimum(min);
    if (verbose > 0) std::cout << *m_min;
    if (!min.IsValid()) {
      throw Exception("Minuit abnormal termination.  No convergence?");
    }
    m_distance = min.Edm();
    std::vector<double> ParamValues;
    unsigned int i = 0;
    for (pptr p = params.begin(); p != params.end(); p++, i++) {
      ParamValues.push_back(m_min->UserParameters().Value(i));
    }
    m_stat->setFreeParamValues(ParamValues);
  }

  // Call Minuit's HESSE to get a robust estimate of the covariance matrix
  void NewMinuit::hesse(int verbose) {
    if (!m_min) 
      throw Exception("Minuit: find_min must be executed before hesse");
    ROOT::Minuit2::MnHesse hesse(m_strategy);
#if ROOT_SVN_REVISION > 23900
    hesse(m_FCN, *m_min, m_maxEval);
#else
    m_min->UserState() = hesse(m_FCN, m_min->UserParameters(), m_maxEval);
#endif
    if (verbose > 0) std::cout << m_min->UserState();
    if (!m_min->HasValidCovariance())
      throw Exception("Minuit HESSE results invalid");
  }

  // Call MINOS
  std::pair<double,double> NewMinuit::Minos(unsigned int n) {
    unsigned int npar = m_min->UserParameters().Params().size();
    if (n >= npar) {
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
  std::cout << "myFCN params: " << params[0] << " " << params[1] << ", value: " << -m_stat->value() << std::endl;
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
    std::cout << " gradient: " << grad[0] << " " << grad[1] << std::endl;
    return grad;
  }

  // Get the uncertainty values from covariance matrix
   const std::vector<double> & NewMinuit::getUncertainty(bool useBase) {
      if (useBase) {
         Optimizer::getUncertainty(useBase);
      } else {
         if (!m_min->HasValidCovariance()) {
            hesse(0);
         }
         m_uncertainty.clear();
         for (size_t i = 0; i < m_min->UserParameters().Params().size(); i++) {
            m_uncertainty.push_back(sqrt(m_min->UserCovariance()(i, i)));
         }
      }
      return m_uncertainty;
   }

  std::ostream& NewMinuit::put (std::ostream& s) const {
    s << m_min->UserState();
    return s;
  }

   std::vector<std::vector<double> > NewMinuit::covarianceMatrix() const {
      if (!m_min->HasValidCovariance()) {
         const_cast<NewMinuit *>(this)->hesse(0);
      }
      std::vector<std::vector<double> > covariancematrix;
      
      ROOT::Minuit2::MnUserCovariance cov = m_min->UserCovariance();
      
      for (unsigned int x = 0; x < cov.Nrow(); ++x) {
         std::vector<double> vec;
         for (unsigned int y = 0; y < cov.Nrow(); ++y) {
            vec.push_back(cov(x,y));
         }
         covariancematrix.push_back(vec);
      }
      
      return covariancematrix;
   }

   void NewMinuit::setTolerance(double tol, int tolType) {
      m_tolerance = 1000. * tol / m_FCN.Up();
      if (tolType == RELATIVE) {
         m_tolerance *= fabs(m_stat->value());
      }
   }

} // namespace optimizers
