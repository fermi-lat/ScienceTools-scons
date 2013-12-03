// @(#)root/minuit2:$Id$
// Authors: M. Winkler, F. James, L. Moneta, A. Zsenei   2003-2005  

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2005 LCG ROOT Math team,  CERN/PH-SFT                *
 *                                                                    *
 **********************************************************************/

#include <stdexcept>

//#include "Minuit2/MnMinos.h"
#include "StMnMinos.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/FCNBase.h"
#include "Minuit2/MnFunctionCross.h"
#include "Minuit2/MnCross.h"
#include "Minuit2/MinosError.h"

namespace optimizers {

StMnMinos::StMnMinos(const ROOT::Minuit2::FCNBase & fcn,
                     const ROOT::Minuit2::FunctionMinimum & min,
                     unsigned int stra) 
   : ROOT::Minuit2::MnMinos(fcn, min, stra),
     m_fFCN(fcn), 
     m_fMinimum(min), 
     m_fStrategy(ROOT::Minuit2::MnStrategy(stra)) {
//   std::cout << "Using StMnMinos" << std::endl;
} 

StMnMinos::StMnMinos(const ROOT::Minuit2::FCNBase & fcn,
                     const ROOT::Minuit2::FunctionMinimum & min,
                     const ROOT::Minuit2::MnStrategy & stra)
   : ROOT::Minuit2::MnMinos(fcn, min, stra),
     m_fFCN(fcn),
     m_fMinimum(min),
     m_fStrategy(stra) {
//   std::cout << "Using StMnMinos" << std::endl;
} 

std::pair<double,double>
StMnMinos::operator()(unsigned int par,
                      unsigned int maxcalls,
                      double toler) const {
   // Do Minos analysis given the parameter index returning a pair for
   // (lower, upper) errors
   ROOT::Minuit2::MinosError mnerr = Minos(par, maxcalls, toler);
   return mnerr();
}

double StMnMinos::Lower(unsigned int par, unsigned int maxcalls,
                        double toler) const {
   // Get lower error for parameter par
   ROOT::Minuit2::MnUserParameterState upar = m_fMinimum.UserState();
   double err = m_fMinimum.UserState().Error(par);
   
   ROOT::Minuit2::MnCross aopt = Loval(par, maxcalls, toler);
   
//   double lower = aopt.IsValid() ? -1.*err*(1.+ aopt.Value()) : (aopt.AtLimit() ? upar.Parameter(par).LowerLimit() : upar.Value(par));
   double lower;
   if (aopt.IsValid()) {
      lower = -1.*err*(1. + aopt.Value());
   } else {
      if (aopt.AtLimit()) {
         lower = upar.Parameter(par).LowerLimit();
      } else {
         lower = upar.Value(par);
      }
   }
   
   return lower;
}

double StMnMinos::Upper(unsigned int par, unsigned int maxcalls,
                        double toler) const {
   // Get upper error for parameter par
   ROOT::Minuit2::MnCross aopt = Upval(par, maxcalls,toler);
   
   ROOT::Minuit2::MnUserParameterState upar = m_fMinimum.UserState();
   double err = m_fMinimum.UserState().Error(par);
   
//    double upper = aopt.IsValid() ? err*(1.+ aopt.Value()) : (aopt.AtLimit() ? upar.Parameter(par).UpperLimit() : upar.Value(par));
   double upper;
   if (aopt.IsValid()) {
      upper = -1.*err*(1. + aopt.Value());
   } else {
      if (aopt.AtLimit()) {
         upper = upar.Parameter(par).UpperLimit();
      } else {
         upper = upar.Value(par);
      }
   }
   
   return upper;
}

double StMnMinos::Lower_valid(unsigned int par, unsigned int maxcalls,
                              double toler) const {
   // Get lower error for parameter par
   ROOT::Minuit2::MnUserParameterState upar = m_fMinimum.UserState();
   double err = m_fMinimum.UserState().Error(par);
   
   ROOT::Minuit2::MnCross aopt = Loval(par, maxcalls, toler);
   
   if (!aopt.IsValid()) {
      throw std::runtime_error("Invalid lower error found.");
   }
   if (aopt.AtLimit()) {
      throw std::runtime_error("Search for lower error at parameter limit.");
   }

   double lower = -1.*err*(1.+ aopt.Value());
   
   return lower;
}

double StMnMinos::Upper_valid(unsigned int par, unsigned int maxcalls,
                              double toler) const {
   // Get upper error for parameter par
   ROOT::Minuit2::MnUserParameterState upar = m_fMinimum.UserState();
   double err = m_fMinimum.UserState().Error(par);

   ROOT::Minuit2::MnCross aopt = Upval(par, maxcalls, toler);
   
   if (!aopt.IsValid()) {
      throw std::runtime_error("Invalid upper error found.");
   }

   if (aopt.AtLimit()) {
      throw std::runtime_error("Search for upper error at parameter limit.");
   } 
  
   double upper = err*(1.+ aopt.Value());
   
   return upper;
}

ROOT::Minuit2::MinosError 
StMnMinos::Minos(unsigned int par, unsigned int maxcalls,
                 double toler) const {
   // Do full minos error anlysis (lower + upper) for parameter par 
   assert(m_fMinimum.IsValid());  
   assert(!m_fMinimum.UserState().Parameter(par).IsFixed());
   assert(!m_fMinimum.UserState().Parameter(par).IsConst());
   
   ROOT::Minuit2::MnCross up = Upval(par, maxcalls, toler);
   ROOT::Minuit2::MnCross lo = Loval(par, maxcalls, toler);
   return ROOT::Minuit2::MinosError(par, m_fMinimum.UserState().Value(par),
                                    lo, up);
}


ROOT::Minuit2::MnCross 
StMnMinos::FindCrossValue(int direction, unsigned int par,
                          unsigned int maxcalls, double toler) const {
   // Get crossing value in the parameter direction : 
   // direction = +1 upper value
   // direction = -1 lower value
   // pass now tolerance used for Migrad minimizations

   assert(direction == 1 || direction == -1); 

   assert(m_fMinimum.IsValid());  
   assert(!m_fMinimum.UserState().Parameter(par).IsFixed());
   assert(!m_fMinimum.UserState().Parameter(par).IsConst());
   if(maxcalls == 0) {
      unsigned int nvar = m_fMinimum.UserState().VariableParameters();
      maxcalls = 2*(nvar+1)*(200 + 100*nvar + 5*nvar*nvar);
   }
   
   std::vector<unsigned int> para(1, par);
   
   ROOT::Minuit2::MnUserParameterState upar = m_fMinimum.UserState();
   double err = direction*upar.Error(par);
   double val = upar.Value(par) + err;

   // Enforce bounds on parameter value.
   double my_lower_limit = upar.Parameter(par).LowerLimit();
   double my_upper_limit = upar.Parameter(par).UpperLimit();
   if (val < my_lower_limit) {
      err = my_lower_limit - upar.Value(par);
      val = my_lower_limit;
   } else if (val > my_upper_limit) {
      err = my_upper_limit - upar.Value(par);
      val = my_upper_limit;
   }

   std::vector<double> xmid(1, val);
   std::vector<double> xdir(1, err);
   
   double up = m_fFCN.Up();
   unsigned int ind = upar.IntOfExt(par);
   // get error matrix (methods return a copy)
   ROOT::Minuit2::MnAlgebraicSymMatrix m = m_fMinimum.Error().Matrix();  
   // get internal paramaters 
   const ROOT::Minuit2::MnAlgebraicVector & xt = m_fMinimum.Parameters().Vec();
   // LM:  change to use err**2 (m(i,i) instead of err as in F77 version
   double xunit = sqrt(up/m(ind, ind));
   // LM (29/04/08) bug: change should be done in internal variables 
   for (unsigned int i = 0; i < m.Nrow(); i++) {
      if (i == ind) {
         continue;
      }
      double xdev = xunit*m(ind,i);
      double xnew = xt(i) + direction *  xdev;

      // transform to external values 
      unsigned int ext = upar.ExtOfInt(i);
      
      double unew = upar.Int2ext(i, xnew); 

      upar.SetValue(ext, unew);
   }
   
   upar.Fix(par);
   upar.SetValue(par, val);

   ROOT::Minuit2::MnFunctionCross cross(m_fFCN, upar, m_fMinimum.Fval(),
                                        m_fStrategy);   
   ROOT::Minuit2::MnCross aopt = cross(para, xmid, xdir, toler, maxcalls);
   
   return aopt;
}

ROOT::Minuit2::MnCross StMnMinos::Upval(unsigned int par,
                                        unsigned int maxcalls,
                                        double toler) const {
   // Return crossing in the lower parameter direction
   return FindCrossValue(1, par, maxcalls, toler);
}
   
ROOT::Minuit2::MnCross StMnMinos::Loval(unsigned int par,
                                        unsigned int maxcalls,
                                        double toler) const {
   // Return crossing in the lower parameter direction
   return FindCrossValue(-1, par, maxcalls, toler);
}

} // namespace optimizers
