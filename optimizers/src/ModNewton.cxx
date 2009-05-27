/** 
 * @file ModNewton.cxx
 * @brief ModNewton class implementation
 * @author P. Nolan
 *
 */

#include "optimizers/ModNewton.h"
#include "optimizers/Parameter.h"
#include "optimizers/Exception.h"
#include "optimizers/dArg.h"
#include "optimizers/OutOfBounds.h"
#include <vector>
#include <algorithm>
#include <iostream>

namespace optimizers {

  typedef std::vector<double>::iterator dptr;
  typedef std::vector<Parameter>::iterator pptr;

  std::vector<double> & ModNewton::getUncertainty(bool useBase) {
//      if (useBase) {
//         Optimizer::getUncertainty(useBase);
//      }
     Optimizer::getUncertainty(useBase);
     return m_uncertainty;
  }

  int ModNewton::getRetCode(void) const {
    return m_retCode;
  }

  void ModNewton::setNeedCovariance(bool b) {
    m_NeedCovariance = b;
  }

  void ModNewton::find_min(int verbose, double tol, int tolType) {

    /// Unpack model parameters into the arrays needed by the solvers
    
    std::vector<Parameter> params;
    m_stat->getFreeParams(params);
    const int nparams = params.size();
    
    std::vector<double> paramVals;
    std::vector<double> paramBounds;
    for (pptr p = params.begin(); p != params.end(); p++) {
      paramVals.push_back(p->getValue());
      paramBounds.push_back(p->getBounds().first);
      paramBounds.push_back(p->getBounds().second);
    }
    
    /// Create the variables and arrays used by the solver.
    /// These serve as storage between calls 
    /// so they must be declared outside the loop.
    /// Most of them don't need to be initialized.

    double funcVal;
    const int liv = 59 + nparams;
    const int lv = 77 + nparams*(nparams+23)/2;
    std::vector<double> gradient(nparams), scale(nparams, 1.);
    std::vector<int> iv(liv);
    std::vector<double> v(lv);

    // Set default values for internal settings
    const int kind = 2;
    divset_(&kind, &iv[0], &liv, &lv, &v[0]);
    if (verbose == 0) {iv[20] = 0;}
    if (tolType == RELATIVE) {
      v[31] = tol;
    } else if (tolType == ABSOLUTE) {
      v[31] = 3e-16;
    }
    iv[16] = m_maxEval;

    /// Call the optimizing function in an infinite loop.
    double oldVal = 1.e+30;
    m_evals = 0;
    m_grads = 0;
    for (;;) {
      if (m_useGrad) {
        drmngb_(&paramBounds[0], &scale[0], &funcVal, &gradient[0], &iv[0], 
	      &liv,&lv, &nparams, &v[0], &paramVals[0]);
      } else {
        drmnfb_(&paramBounds[0], &scale[0], &funcVal, &iv[0], 
	      &liv,&lv, &nparams, &v[0], &paramVals[0]);
      }
      int rcode = iv[0];
      if (rcode == 1 || rcode == 2) { /// request for a function or derivative
	try {m_stat->setFreeParamValues(paramVals);}
	catch (OutOfBounds e) {
	  iv[1] = 1;  // Tell it to try a shorter step
	  std::cerr << "ModNewton::find_min error" << std::endl;
	  std::cerr << e.what() << std::endl;
	  std::cerr << "Value " << e.value() << " is not between "
		    << e.minValue() << " and " << e.maxValue() << std::endl;
	  continue;  // Try again
	}
	catch (Exception e) {
	  std::cerr << e.what() << std::endl;
	}
        if (rcode == 1) {
          funcVal = -m_stat->value();
          m_evals++;
          m_val = funcVal;
          if (tolType == ABSOLUTE && iv[28] == 4 && fabs(funcVal-oldVal) < tol) {
            // check after a successful line search
            m_retCode = 6;
            if (verbose != 0)
              std::cout << "***** ABSOLUTE FUNCTION CONVERGENCE x****" 
		      << std::endl;
             break;
	  }
          oldVal = funcVal;
        } else if (rcode == 2) {
          if (m_useGrad) {
	    m_stat->getFreeDerivs(gradient);
            for (dptr p = gradient.begin(); p != gradient.end(); p++) 
              {*p = -*p;}
          } else {
            funcVal = -m_stat->value();
            m_val = funcVal;
          }
          m_grads++;
        }
      } else if (rcode == 9) {
        m_retCode = rcode;
        if (verbose != 0) 
          std::cout << "***** TERMINATED FOR TOO MANY FUNCTION EVALUATIONS ****"
             << std::endl;
        break;
      }
      else {  /// Finished.  Exit loop.
	m_retCode = rcode;
	if (rcode > 6) {throw Exception("ModNewton error", rcode);}
	break;
      }
    }

    /// Get parameter values and put them back into the Function
    m_stat->setFreeParamValues(paramVals);

    if (m_NeedCovariance) {
      /// Get the Cholesky factor of the Hessian.  It's a lower triangular
      /// matrix stored in compact fashion, so we treat it as 1-dimensional.
      const dptr vp = v.begin() + iv[41] - 1;
      std::vector<double> hess(vp, vp + nparams*(nparams+1)/2);
      
      /// Invert the Hessian to produce the covariance matrix.  The result 
      /// is also stored as a compact triangular matrix.
      /// Dpptri and drmngb/drmnfb have opposite definitions of "lower" and
      /// "upper" triangular matrices.  Drmn(f|g)b says it produces a lower
      /// matrix.  If we tell dpptri that it's upper, all is OK.

      int info;
      const char uplo = 'U';
      dpptri_(&uplo, &nparams, &hess[0], &info, 1);
      if (info < 0) 
	throw Exception("DPPTRI: illegal argument value", -info);
      else if (info > 0) 
	throw Exception("DPPTRI: Zero diagonal element in Cholesky factor",
			info);
      
      /// Extract the diagonal elements of the covariance matrix.
      /// Their square roots are the parameter uncertainties.
      m_uncertainty.clear();
      for (int i = 0; i < nparams; i++) {
	m_uncertainty.push_back(sqrt(hess[i*(i+3)/2]));
      }
    }

  } // End of find_min

  void ModNewton::find_min_only(int verbose, double tol, int tolType) {
    setNeedCovariance(false);
    find_min(verbose, tol, tolType);
  }

  std::ostream& ModNewton::put (std::ostream& s) const {
    s << "ModNewton performed " << m_evals << " function evaluations" << std::endl;
    s << "and " << m_grads << " gradient evaluations, ending with" << std::endl;
    s << "a function value of " << m_val << std::endl;
    return s;
  }

} // namespace optimizers
