/**
 * @file NewMinuit.cxx
 * @brief Implementation of the NewMinuit class
 * @author P. Nolan
 * $Header$
 */

#include <sstream>
#include "optimizers/NewMinuit.h"
#include "optimizers/Parameter.h"
#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/MnMinimize.h"
#include "Minuit2/MnMinos.h"
#include "Minuit2/MnContours.h"
#include "Minuit2/ContoursError.h"
#include "Minuit2/MnPlot.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnHesse.h"
#include "Minuit2/MnPrint.h"
#include "optimizers/Exception.h"
#include "optimizers/OutOfBounds.h"
#include "StMnMinos.h"
#ifndef BUILD_WITHOUT_ROOT
#include "RVersion.h"
#endif

namespace optimizers {

  typedef std::vector<Parameter>::iterator pptr;

  // Constructor
  NewMinuit::NewMinuit(Statistic & stat) 
  : Optimizer(stat), m_FCN(stat), 
    m_tolerance(1e-3), m_strategy(ROOT::Minuit2::MnStrategy(1)), m_min(0),
    m_strategy_value(1) {}

  //  =
  NewMinuit & NewMinuit::operator=(const NewMinuit & rhs) {
      if (this == &rhs) return *this;
      Optimizer::operator=(rhs);
      m_FCN = rhs.m_FCN;
      m_distance = rhs.m_distance;
      m_tolerance = rhs.m_tolerance;
      m_strategy = rhs.m_strategy;
      delete m_min;
      m_min = new ROOT::Minuit2::FunctionMinimum(*(rhs.m_min));
      m_strategy_value = rhs.m_strategy_value;
      return *this;
  }

  //Copy constructor
  NewMinuit::NewMinuit(const NewMinuit & x) 
     : Optimizer(x), 
       m_FCN(x.m_FCN), m_distance(x.m_distance), m_tolerance(x.m_tolerance),
       m_strategy(x.m_strategy), m_strategy_value(x.m_strategy_value) {
     m_min = new ROOT::Minuit2::FunctionMinimum(*(x.m_min));
  }

  // Call Minuit's MIGRAD to find the minimum of the function
  int NewMinuit::find_min(int verbose, double tol, int TolType) {
    find_min_only(verbose, tol, TolType);
    std::vector<double> parValues;
    m_stat->getFreeParamValues(parValues);
    hesse(verbose);
    m_stat->setFreeParamValues(parValues);
    return getRetCode();
  }

  int NewMinuit::find_min_only(int verbose, double tol, int TolType) {
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
    ROOT::Minuit2::MnMinimize migrad(m_FCN, userState, m_strategy);
    ROOT::Minuit2::FunctionMinimum min = migrad(m_maxEval, m_tolerance);
    delete m_min;
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
    setRetCode(checkResults());
    return getRetCode();
  }

  // Call Minuit's HESSE to get a robust estimate of the covariance matrix
  void NewMinuit::hesse(int verbose) {
    if (!m_min) 
      throw Exception("Minuit: find_min must be executed before hesse");
    ROOT::Minuit2::MnHesse hesse(m_strategy);
#ifndef BUILD_WITHOUT_ROOT
#if ROOT_SVN_REVISION > 23900
    hesse(m_FCN, *m_min, m_maxEval);
#else
    m_min->UserState() = hesse(m_FCN, m_min->UserParameters(), m_maxEval);
#endif
#else
    hesse(m_FCN, *m_min, m_maxEval);
#endif
    if (verbose > 0) std::cout << m_min->UserState();
    if (!m_min->HasValidCovariance())
      throw Exception("Minuit HESSE results invalid");
  }

  // Call MINOS
  std::pair<double,double> NewMinuit::Minos(unsigned int n, double level) {
    std::vector<double> parValues;
    checkParValues(n, parValues);
    if(level!=1.){
      m_FCN.SetErrorDef(level/2.);
      m_min->SetErrorDef(level/2.);
    }
//    ROOT::Minuit2::MnMinos mns(m_FCN, *m_min, m_strategy);
    StMnMinos mns(m_FCN, *m_min, m_strategy);
    ROOT::Minuit2::MinosError minos_error(mns.Minos(n));
    if (!minos_error.LowerValid()) {
       std::ostringstream message;
       message << "Minuit2::MnMinos could not find valid lower limit for "
               << "parameter number " << n;
       throw Exception(message.str());
    }
    if (!minos_error.UpperValid()) {
       std::ostringstream message;
       message << "Minuit2::MnMinos could not find valid upper limit for "
               << "parameter number " << n;
       throw Exception(message.str());
    }
    std::pair<double,double> results(minos_error());
    //Back to default
    if(level!=1.){
      m_FCN.SetErrorDef(0.5);
      m_min->SetErrorDef(0.5);
    }
    m_stat->setFreeParamValues(parValues);
    return results;
  }

   double NewMinuit::minos_lower_error(unsigned int n, double level,
                                       double tol) {
      std::vector<double> parValues;
      checkParValues(n, parValues);
      if (level != 1) {
         m_FCN.SetErrorDef(level/2.);
         m_min->SetErrorDef(level/2.);
      }
      StMnMinos minos(m_FCN, *m_min, m_strategy);
      unsigned int maxcalls;
      double toler;
      double lower = minos.Lower_valid(n, maxcalls=0, toler=tol);
      // Reset to defaults.
      if (level != 1.) {
         m_FCN.SetErrorDef(0.5);
         m_min->SetErrorDef(0.5);
      }
      m_stat->setFreeParamValues(parValues);
      return lower;
   }

   double NewMinuit::minos_upper_error(unsigned int n, double level, 
                                       double tol) {
      std::vector<double> parValues;
      checkParValues(n, parValues);
      if (level != 1) {
         m_FCN.SetErrorDef(level/2.);
         m_min->SetErrorDef(level/2.);
      }
      StMnMinos minos(m_FCN, *m_min, m_strategy);
      unsigned int maxcalls;
      double toler;
      double upper = minos.Upper_valid(n, maxcalls=0, toler=tol);
      // Reset to defaults.
      if (level != 1.) {
         m_FCN.SetErrorDef(0.5);
         m_min->SetErrorDef(0.5);
      }
      m_stat->setFreeParamValues(parValues);
      return upper;
   }
      
   void NewMinuit::checkParValues(unsigned int n,
                                  std::vector<double> & parValues) const {
      m_stat->getFreeParamValues(parValues);
      unsigned int npar = m_min->UserParameters().Params().size();
      if (n >= npar) {
         throw Exception("Parameter number out of range in Minos", n);
      }
   }

  // Call MNCONTOUR
  void NewMinuit::MnContour(unsigned int par1, unsigned int par2,
			 double level, unsigned int npts) {
    std::vector<double> parValues;
    m_stat->getFreeParamValues(parValues);
    unsigned int npar = m_min->UserParameters().Params().size();
    if (par1 >= npar) {
      throw Exception("Parameter number out of range in MnContour", par1);
    }
    if (par2 >= npar) {
      throw Exception("Parameter number out of range in MnContour", par2);
    }
    ROOT::Minuit2::MnContours mnc(m_FCN, *m_min, m_strategy);
    if(level!=1.){
      m_FCN.SetErrorDef(level/2.);
      m_min->SetErrorDef(level/2.);
    }
    //std::vector<std::pair<double,double> > results = mnc(par1,par2,npts);
    //ROOT::Minuit2::MnPlot mnp;
    //mnp(results);
    ROOT::Minuit2::ContoursError conterr=mnc.Contour(par1,par2,npts);
    std::cout<<conterr<<std::endl;
    //Back to default
    if(level!=1.){
      m_FCN.SetErrorDef(0.5);
      m_min->SetErrorDef(0.5);
    }
    m_stat->setFreeParamValues(parValues);
    return;
  }
  // Constructor for the function to be minimized
  myFCN::myFCN(Statistic & stat): m_stat(&stat), m_level(0.5) {}

  // This is the function that Minuit minimizes
  double 
  myFCN::operator() (const std::vector<double> & params) const {
    try {m_stat->setFreeParamValues(params);}
    catch (OutOfBounds & e) {
      std::cerr << e.what() << std::endl;
      std::cerr << "Value " << e.value() << " is not between "
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
      std::vector<double> parValues;
      m_stat->getFreeParamValues(parValues);
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
      m_stat->setFreeParamValues(parValues);
      return m_uncertainty;
   }

  std::ostream& NewMinuit::put (std::ostream& s) const {
    s << m_min->UserState();
    return s;
  }

   std::vector<std::vector<double> > NewMinuit::covarianceMatrix() const {
      std::vector<double> parValues;
      m_stat->getFreeParamValues(parValues);
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
      m_stat->setFreeParamValues(parValues);
      return covariancematrix;
   }

   void NewMinuit::setTolerance(double tol, int tolType) {
      m_tolerance = 1000. * tol / m_FCN.Up();
      if (tolType == RELATIVE) {
         m_tolerance *= fabs(m_stat->value());
      }
   }
 
   int NewMinuit::checkResults() {
      if (m_min->HasReachedCallLimit()) return 1;

      // Set a bit for each good condition.
      unsigned int code = 0;
      if (m_min->IsValid()) code |= 1;
      code <<= 1;
      if (m_min->HasValidParameters()) code |= 1;
      code <<= 1;
      if (m_min->HasValidCovariance()) code |= 1;
      code <<= 1;
      if (m_min->HasAccurateCovar()) code |= 1;
      code <<= 1;
      if (m_min->HasPosDefCovar()) code |= 1;
      code <<= 1;
      if (! m_min->HasMadePosDefCovar()) code |= 1;
      code <<= 1;
      if (! m_min->HesseFailed()) code |= 1;
      code <<= 1;
      if (m_min->HasCovariance()) code |= 1;
      code <<= 1;
      if (! m_min->IsAboveMaxEdm()) code |= 1;
    
      if (code == 0x1FF) return 0;  // All good!
      code = 0x1FF & ~ code;  // Return a bitmap of the bad conditions.
      return code+100;
   }

} // namespace optimizers
