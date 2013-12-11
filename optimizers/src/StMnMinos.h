/**
 * @file StMnMinos.h
 *
 * @brief Modifications of Minuit2::MnMinos to add Lower_valid and
 * Upper_valid member functions and to enforce user-specified bounds
 * on parameters and resulting scaling differences applied in
 * MnFunctionCross.
 *
 * $Header$ 
 */

#ifndef optimizers_StMnMinos_h
#define optimizers_StMnMinos_h

#include <iostream>
#include "Minuit2/MnMinos.h"

namespace optimizers {

class StMnMinos : public ROOT::Minuit2::MnMinos {

public:

   StMnMinos(const ROOT::Minuit2::FCNBase & fcn,
             const ROOT::Minuit2::FunctionMinimum & min,
             unsigned int strat=1);

   StMnMinos(const ROOT::Minuit2::FCNBase & fcn,
             const ROOT::Minuit2::FunctionMinimum & min,
             const ROOT::Minuit2::MnStrategy & stra);

   ~StMnMinos() {}
  
   /// Returns the negative (pair.first) and the positive (pair.second) 
   /// Minos Error of the Parameter
   std::pair<double, double> operator()(unsigned int, unsigned int maxcalls=0,
                                        double toler=0.01) const;

   /// Calculate one side (negative or positive Error) of the Parameter
   /// give as input (optionally) maxcalls and tolerance
   double Lower(unsigned int, unsigned int maxcalls=0, 
                double toler=0.01) const;
   double Upper(unsigned int, unsigned int maxcalls=0,
                double toler=0.01) const;

   /// Return only valid lower or upper limits.  Throw exceptions if
   /// invalid.
   double Lower_valid(unsigned int, unsigned int maxcalls=0, 
                double toler=0.01) const;
   double Upper_valid(unsigned int, unsigned int maxcalls=0, 
                double toler=0.01) const;

   ROOT::Minuit2::MnCross Loval(unsigned int, unsigned int maxcalls=0,
                                double toler=0.01) const;
   ROOT::Minuit2::MnCross Upval(unsigned int, unsigned int maxcalls=0,
                                double toler=0.01) const;

   /// Ask for MinosError (Lower + Upper)
   /// can be printed via std::cout  
   ROOT::Minuit2::MinosError Minos(unsigned int, unsigned int maxcalls=0,
                                   double toler=0.01) const;

private:

   const ROOT::Minuit2::FCNBase & m_fFCN;
   const ROOT::Minuit2::FunctionMinimum & m_fMinimum;
   ROOT::Minuit2::MnStrategy m_fStrategy;
   mutable bool m_lower_rescaled;
   mutable bool m_upper_rescaled;

   /// Get crossing value via MnFunctionCross
   ROOT::Minuit2::MnCross FindCrossValue(int dir , unsigned int,
                                         unsigned int maxcalls,
                                         double toler) const;

};

} // namespace optimizers

#endif // optimizers_StMnMinos_h
