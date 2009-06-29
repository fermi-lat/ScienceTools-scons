/** 
 * @file Minuit.cxx
 * @brief Minuit class implementation
 * @author P. Nolan
 *
 * $Header$
 */

#include <sstream>
#include "optimizers/dArg.h"
#include "optimizers/Minuit.h"
#include "optimizers/Parameter.h"
#include "optimizers/Exception.h"
#include "optimizers/OutOfBounds.h"

namespace {
   int numPars(0);
}

namespace optimizers {


  Minuit::Minuit(Statistic& stat) : Optimizer(stat) {
    const integer i5=5, i6=6, i7=7;
    mninit_(&i5, &i6, &i7);
  }

  int Minuit::getQuality(void) const {
    return m_quality;
  }

  double Minuit::getDistance(void) const {
    return m_distance;
  }

  void Minuit::setStrategy(unsigned int strat) {
      std::ostringstream s_strategy;
      s_strategy << "SET STRATEGY " << strat;
      doCmd(s_strategy.str());
  }

  const std::vector<double> & Minuit::getUncertainty(bool useBase) {
     if (useBase) {
        Optimizer::getUncertainty(useBase);
     }
     return m_uncertainty;
  }

  int Minuit::find_min(int verbose, double tol, int tolType) {
    return minimize(verbose, tol, tolType, true);
  }

  int Minuit::find_min_only(int verbose, double tol, int tolType) {
    return minimize(verbose, tol, tolType, false);
  }

  int Minuit::minimize(int verbose, double tol, int tolType, bool doHesse) {

    typedef std::vector<Parameter>::iterator pptr;

    std::vector<Parameter> params;
    m_stat->getFreeParams(params);
    numPars = params.size();
    integer errorFlag;

    int minuitVerbose = verbose - 1;
    if (minuitVerbose >= 0) {
      const integer i5=5, i6=6, i7=7;
      mintio_(&i5, &i6, &i7);
    }
    std::ostringstream pline;
    pline << "SET PRINT " << minuitVerbose;
    doCmd(pline.str()); // Set verbosity of Minuit
    doCmd("SET NOWARN");

    // Tell Minuit about parameter values, names, bounds, etc.
    for (pptr p = params.begin(); p != params.end(); p++) {
      double scale = 1.0; // Is this the best choice?
      double value = p->getValue();
      double lowerBound = p->getBounds().first;
      double upperBound = p->getBounds().second;
      integer j = p - params.begin() + 1;
      mnparm_(&j, p->getName().c_str(), &value, &scale, 
	      &lowerBound, &upperBound, &errorFlag, p->getName().size());
    }

    doCmd("SET ERR 0.5");  // Delta value = 1/2: correct for likelihood
    doCmd("SET GRAD 1");  // Use gradient calculated by fcn
    
    double tolerance = 0.;
    if (tolType == ABSOLUTE) {
      tolerance = 2000. * tol;
    } else if (tolType == RELATIVE) {
      tolerance = 2000. * tol * fabs(m_stat->value());
    }
    std::ostringstream mline;
    mline << "MINIMIZE " << m_maxEval << " " << tolerance << std::endl;
    int retCode = doCmd(mline.str());  // Minimize fcn

    // Improve the quality of the Hessian matrix.
    if (doHesse) doCmd("HESSE");

    // Extract fitted parameters
    if (verbose != 0) {
      std::cout << "Final values: " << std::endl;
    }
    for (pptr p = params.begin(); p != params.end(); p++) {
      std::vector<char> pname(10);
      double pval, error, bnd1, bnd2;
      integer ivarbl;
      integer j = p - params.begin() + 1;
      mnpout_(&j, &pname[0], &pval, &error, &bnd1, &bnd2, &ivarbl, 
	      pname.size());
      p->setValue(pval);
      if (verbose != 0) {
	std::cout << "  " << std::string(pname.begin(), pname.end()) 
		  << " = " << pval << std::endl;
      }
    }

    // Put new parameter values back into the Statistic
    std::vector<double> paramValues;
    for (unsigned int i = 0; i < params.size(); i++) {
      paramValues.push_back(params[i].getValue());
    }
    m_stat->setFreeParamValues(paramValues);

    // Get information about quality of minimization
    integer nVariable, nparx, minStat;
    double fmin, vertDist, errDef;
    mnstat_(&fmin, &vertDist, &errDef, &nVariable, &nparx, &minStat);
    m_val = fmin;
    m_quality = minStat;
    m_distance = vertDist;
    if (verbose != 0) {
      std::cout << "Minuit fit quality: " << minStat << 
	"   estimated distance: " << vertDist << std::endl;
    }

    // Get parameter uncertainties
    if (verbose != 0) {
      std::cout << "Minuit parameter uncertainties:" << std::endl;
    }
    m_uncertainty.clear();
    for (integer i = 1; i <= nVariable; i++) {
      double ePlus, eMinus, eParab, globCC;
      mnerrs_(&i, &ePlus, &eMinus, &eParab, &globCC);
      m_uncertainty.push_back(eParab);  // Not using MINOS, so this is it.
      if (verbose != 0) {
	std::cout << "  " << i << "  " << eParab << std::endl;
      }
    }

    if (retCode == 0 && minStat == 3) {
      setRetCode(0);    // OK!
    } else if (retCode == 4) {
      setRetCode(1);  // Abnormal termination.  Assume it's too many iterations.
    } else {
      if (retCode == 0) {
        setRetCode(100+minStat);  // Bad covariance.
      } else {
        setRetCode(110);   // Some sort of bad command.
      }
    }
    return getRetCode();
  } // End of minimize 

  std::pair<double,double> Minuit::Minos(unsigned int n) {
    integer npar(m_stat->getNumFreeParams());
//     if (n < 1 || n > npar) {
//       throw Exception("Parameter number out of range in Minos", n);
//     }
    /// For the public interface, assume the parameter numbering
    /// starts with zero (as with the Minuit2 MnMinos interface).  So
    /// we test the range of n accordingly and add one before calling
    /// the Fortran-based code.
    if (n >= static_cast<unsigned int>(npar)) {
      throw Exception("Parameter number out of range in Minos", n);
    }
    n += 1;
    std::ostringstream mcmd;
    mcmd << "MINOS " << m_maxEval << " " << n;
    numPars = npar;
    doCmd(mcmd.str());
    double eplus, eminus, eparab, globcc;
    integer my_n = n;
    mnerrs_(&my_n, &eplus, &eminus, &eparab, &globcc);
    numPars = 0;
    return std::pair<double,double>(eminus,eplus);
  }

  int Minuit::doCmd(std::string command) {
    // Pass a command string to Minuit
    integer errorFlag = 0;
    void * statistic = static_cast<void *>(m_stat);
    mncomd_(&fcn, command.c_str(), &errorFlag, statistic,
	    command.length());
    return errorFlag;
  }

  void fcn(integer* npar, double* grad, double* fcnval,
	   double* xval, integer* iflag, void* futil) {
    // This is the function that Minuit minimizes

     if (numPars == 0) {
        numPars = *npar;
     }

    std::vector<double> parameters(xval, xval + numPars);

    // What a hack!  Minuit thinks futil is a function 
    // pointer.  It's been hijacked to be a pointer to
    // m_stat, so this non-member function can use it.

    Statistic * statp = static_cast<Statistic *>(futil);
    statp->setFreeParamValues(parameters);

    *fcnval = -statp->value();
    if (*iflag == 2) { // Return gradient values
      std::vector<double> gradient;
      statp->getFreeDerivs(gradient);
      for (int i=0; i < *npar; i++) {
	grad[i] = -gradient[i];
      }
    }
  }    

   std::vector< std::vector<double> > Minuit::covarianceMatrix() const {
      integer npar(m_stat->getNumFreeParams());
      std::vector<double> entries(npar*npar);
      mnemat_(&entries[0], &npar);
      size_t indx(0);
      std::vector< std::vector<double> > matrix;
      for (int i(0); i < npar; i++) {
         std::vector<double> row;
         for (int j(0); j < npar; j++, indx++) {
            row.push_back(entries.at(indx));
         }
         matrix.push_back(row);
      }
      return matrix;
   }

  std::ostream& Minuit::put (std::ostream& s) const {
    s << "MINUIT returned a value of " << m_val << std::endl;
    s << "and an estimated distance of " << m_distance << std::endl;
    return s;
  }

} // namespace optimizers


