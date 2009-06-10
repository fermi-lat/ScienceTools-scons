/** 
 * @file Lbfgs.cxx
 * @brief Lbfgs class implementation
 * @author P. Nolan
 *
 * $Header$
 */

#include <algorithm>
#include <iostream>
#include <vector>

#include "optimizers/Lbfgs.h"
#include "optimizers/Parameter.h"
#include "optimizers/Exception.h"
#include "optimizers/dArg.h"

namespace optimizers {
  
  void Lbfgs::setMaxVarMetCorr(const int m)
  {m_maxVarMetCorr = m;}
  
  void Lbfgs::setPgtol(const double pgtol)
  {m_pgtol = pgtol;}

  std::string Lbfgs::getErrorString(void) const
  {return m_errorString;}

  int Lbfgs::find_min_only(int verbose, double tol, int tolType) {
    return find_min(verbose, tol, tolType);
  }

  int Lbfgs::find_min(int verbose, double tol, int tolType) {

    m_numEvals = 0;
    m_errorString.erase();

    // Unpack model parameters into the arrays needed by LBFGS
    
    std::vector<Parameter> params;
    m_stat->getFreeParams(params);
    const int nparams = params.size();
    
    std::vector<double> paramVals(nparams);
    std::vector<double> paramMins(nparams);
    std::vector<double> paramMaxs(nparams);
    int i=0;
    for (std::vector<Parameter>::iterator p = params.begin();
	 p != params.end(); p++, i++) {
      paramVals[i] = p->getValue();
      paramMins[i] = p->getBounds().first;
      paramMaxs[i] = p->getBounds().second;
    }
    
    // Create the variables and arrays used by LBFGS
    // These serve as storage between calls to setulb_,
    // so they must be declared outside the loop.
    // Most of them don't need to be initialized.

    double funcVal;
    std::vector<double> gradient(nparams);
    std::vector<int> isave(44);
    std::vector<logical> lsave(4);
    std::vector<double> dsave(29);
    std::vector<char> csave(60, ' ');  // Blank-filled
    std::vector<int> intWorkArray(3*nparams);
    const int workSize = (2*m_maxVarMetCorr + 4) * nparams
      + 12 * m_maxVarMetCorr * (m_maxVarMetCorr + 12);
    std::vector<double> doubleWorkArray(workSize);

    // Fortran-style string used for communication with LBFGS

    std::vector<char> task(60, ' '); // Blank-filled
    static const std::string strt("START");
    std::copy(strt.begin(), strt.end(), task.begin()); // Initialize

    double factr = dpmeps_();
    if (tolType == RELATIVE) factr *= tol;

    // Call LBFGS in an infinite loop.  Break out when it's done.
    double oldVal = 0.;
    for (;;) {
      int iprint = verbose - 2;  
      const std::vector<int> nbd(nparams, 2); // All params bounded for now
      setulb_(&nparams, &m_maxVarMetCorr, &paramVals[0], &paramMins[0], 
	      &paramMaxs[0], &nbd[0], &funcVal, &gradient[0], 
	      &factr, &m_pgtol, &doubleWorkArray[0], &intWorkArray[0], 
	      &task[0], &iprint, &csave[0], &lsave[0], &isave[0], &dsave[0], 
	      task.size(), csave.size());
      std::string taskString(task.begin(), task.end());
      int taskLength = taskString.find_last_not_of(' ') + 1;
      taskString.erase(taskLength); // Strip trailing blanks

      if (taskString.substr(0,2) == "FG") {
	// Request for values of function and gradient.
	// LBFGS is a minimizer, so we must flip the signs to maximize.
	m_stat->setFreeParamValues(paramVals);
	funcVal = -m_stat->value();
	m_stat->getFreeDerivs(gradient);
	for (int i = 0; i < nparams; i++) {
	  gradient[i] = -gradient[i];
	}
	m_numEvals++;
	m_val = funcVal;
	
	if (verbose != 0) {
           std::cout << m_numEvals << "  "
                     << funcVal << std::endl;
//            std::cout << "LBFGS " << funcVal << "\n";
//            for (int i = 0; i < nparams; i++) {
//               std::cout << i << ": "
//                         << paramVals[i] << "  " 
//                         << gradient[i] << "\n";
//            }
	}
      }  // Don't break.  Call setulb_ again
      else if (taskString.substr(0, 5) == "NEW_X") {
	// Ready to move to a new set of parameter values
	if (isave[33] > m_maxEval) {
	  setRetCode(LBFGS_TOOMANY);
	  m_errorString = "Exceeded Specified Number of Iterations";
	  break;
	}
	if (tolType == ABSOLUTE && oldVal != 0. && fabs(funcVal-oldVal) < tol) {
	  setRetCode(LBFGS_NORMAL);
	  m_errorString = "Absolute Convergence";
	  break;
	}
	oldVal = funcVal;
      }  // Otherwise don't break.  Call setulb_ again.
      else if (taskString.substr(0, 4) == "CONV") {
	// Normal convergence
	setRetCode(LBFGS_NORMAL);
	m_errorString = taskString;
	break;
      }
      else if (taskString.substr(0, 4) == "ABNO") {
	// Abnormal termination in line search
	setRetCode(LBFGS_ABNO);
	m_errorString = taskString;
	throw Exception(taskString, LBFGS_ABNO);
      }
      else if (taskString.substr(0, 5) == "ERROR") {
	// Error in input parameters
	setRetCode(LBFGS_ERROR);
	m_errorString = taskString;
	throw Exception(taskString, LBFGS_ERROR);
      }
      else {
	// Something else
	setRetCode(LBFGS_UNKNOWN);
	m_errorString = "LBFGS unknown condition";
	throw Exception(taskString, LBFGS_UNKNOWN);
      }
    }  // End of infinite loop

    // Get parameter values
    int j = 0;
    for (std::vector<Parameter>::iterator p = params.begin();
	 p != params.end(); p++, j++) {
      p->setValue(paramVals[j]);
    }
    // Put parameter values back into the objective Function
    m_stat->setFreeParamValues(paramVals);
    return getRetCode();
  } // End of find_min

  std::ostream& Lbfgs::put (std::ostream& s) const {
    s << "LBFGS returned a function value of " << m_val << std::endl;
    s << "after " << m_numEvals << " evaluations." << std::endl;
    return s;
  }

}
