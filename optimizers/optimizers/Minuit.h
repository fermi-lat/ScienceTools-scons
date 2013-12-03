/** 
 * @file Minuit.h
 * @brief Declaration for the Minuit Optimizer subclass.
 * @author P. Nolan
 * $Header$
 */

#ifndef optimizers_MINUIT_H
#define optimizers_MINUIT_H

#include "optimizers/Optimizer.h"
#include "optimizers/Statistic.h"
#include "optimizers/f2c_types.h"

namespace optimizers {

  /** 
   * @class Minuit
   *
   * @brief Wrapper class for the Minuit optimizer from CERN
   *
   * @author P. Nolan
   *    
   This class implements an Optimizer by using Minuit, a
   well-known package from CERN.  It uses only a few of Minuit's
   features.  It uses only the MIGRAD algorithm.  All variables
   are treated as bounded.  No user interaction is allowed.
   */
  
  // Doxygen the C file here so it can be left as nearly as
  // possible in its pristine, machine-produced state.
  /**
   * @file minuit_routines.c
   *
   * @brief The Minuit package translated from Fortran by f2c
   *
   Minuit is a well-known optimizing/fitting package in the HEP
   community.  It has been developed for over 30 years at CERN.
   
   This file was produced from the CERN Fortran source code.
   First, g77 -E was used to insert all the \#include files and
   make a single, large Fortran file free of preprocessor commands.
   Then f2c -C++ produced this file.  The only hand modification
   required was to change \#include "f2c.h" to \#include "f2c/f2c.h"
   to conform to the way CMT wants files to be laid out.
   
   In non-interactive mode, the API for using Minuit involves the
   functions mninit_, mnseti_, mnparm_, mnpars_, mnexcm_, mncomd_,
   mnpout_, mnstat_, mnemat_, mnerrs_, mncont_, mnintr_, and mninpu_.
  */
  
  class Minuit : public Optimizer {
    
  public:
    
    Minuit(Statistic &stat);
    
    virtual ~Minuit() {}
    
    int find_min(int verbose = 0, double tol = 1e-3, int tolType = ABSOLUTE);
    int find_min_only(int verbose = 0, double tol = 1e-3, int tolType = ABSOLUTE);
    // int minimize(int verbose, double tol, int tolType, bool doHesse);

    //! Minuit return status.   3=OK, 2=forced positive def., 1= not accurate
    int getQuality(void) const;
 
    //! Estimated vertical distance from minimum
    double getDistance(void) const; 

    //! One-sigma confidence regions based on Hessian, assuming 
    // that this function is a likelihood
    virtual const std::vector<double> & getUncertainty(bool useBase=false);

     /// Access to the covariance matrix
     virtual std::vector< std::vector<double> > covarianceMatrix() const;

     /// Run a MINOS error analysis
     std::pair<double,double> Minos(unsigned int n, double level=1.);

    /// Run a MNCONTOUR dynamic CONTOUR analysis
    void MnContour(unsigned int par1, unsigned int par2,
		   double level=1., unsigned int npts=20);
     
    virtual std::ostream& put (std::ostream& s) const;

    //! Symbolic form of the return codes for readability 
    enum MinuitQuality {MINUIT_NOTCALC, MINUIT_DIAG, MINUIT_FORCEDPOS, 
			MINUIT_NORMAL};

    /// Set the minimization strategy
    void setStrategy(unsigned int strat = 1); 

     unsigned int getStrategy() const {
        return m_strategy_value;
     }

  private:
    
    int minimize(int verbose, double tol, int tolType, bool doHesse);

    //! Pass a command string to Minuit
    int doCmd(std::string command);
    int m_quality;
    double m_distance;
    double m_val;

     unsigned int m_strategy_value;

  };
  
  //! The function which Minuit will minimize
  void fcn(integer * npar, double* grad, double* fcnval,
	   double* xval, integer * iflag, void* futil);
} // namespace optimizers

#ifndef SWIG
// The Fortran subroutines which make up the Minuit API
extern "C" {
  //! Initialize Minuit with I/O unit numbers for in, out, save
  void mninit_(const integer*, const integer*, const integer*);
  //! Define a parameter, assigning values and bounds
  void mnparm_(integer *  num, const char * chnam, double * stval, 
	        double * step,  double * bnd1 , 
	        double * bnd2, integer * ierror, ftnlen stringlen);
  //! Prototype of the function to be minimized.
  typedef void (mfcn)(integer * npar, double * grad, double * fval, 
		      double * xval, integer * iflag, void * futil);
  //! Execute a Minuit command specified as a character string
  void mncomd_(mfcn * fcn, const char * chstr, integer * ierr, void * futil, 
	       ftnlen stringlen);
  //! Execute a Minuit command
  void mnexcm_(mfcn * fcn, char * chcom, double * arglis, integer * narg, 
	       integer * ierflg, void * futil, ftnlen strln);
  //! Set I/O unit numbers
  void mintio_(const integer * iread, const integer * iwrite, const integer * isave);
  //! Get current value of a parameter
  void mnpout_(integer * num, char * chnam, double * val, double * error, 
	       double * bnd1, double * bnd2, integer * ivarbl, ftnlen strln);
  //! Get current status of minimization
  void mnstat_(double * fmin, double * fedm, double * errdef, integer * npari, 
	       integer * nparx, integer * istat);
  //! Specify a title for a problem
  void mnseti_(char * ctitle, ftnlen strln);
  //! Define a parameter, assigning values and bounds from variables
  void mnpars_(char * chstr, integer * icondn, ftnlen strln);
  //! Get current value of covariance matrix
  void mnemat_(double * emat, integer * ndim);
  //! Access current parameter errors
  void mnerrs_(integer * num, double * eplus, double * eminus, double * eparab, 
	       double * globcc);
  //! Find a function contour with the MNContour method
  void mncont_(mfcn * fcn, integer * num1, integer * num2, integer * npt, double * xpt, 
	       double * ypt, integer * nfound, void * futil);
  //! Utility function used by Minuit: interactive or batch mode
  logical intrac_(double *);
}
#endif // SWIG

#endif // optimizers_MINUIT_H
