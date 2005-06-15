/** 
 * @file ChiSq.cxx
 * @brief Implementation for the ChiSq class
 * @author James Peachey
 *
 * $Header$
 */

#include <cmath>
#include <stdexcept>

#include "optimizers/Arg.h"
#include "optimizers/ChiSq.h"
#include "optimizers/Function.h"
#include "optimizers/Parameter.h"
#include "optimizers/dArg.h"

namespace optimizers {

ChiSq::ChiSq(const DataCont_t & domain, const DataCont_t & range, optimizers::Function * func):
   m_domain(&domain), m_range(&range), m_func(func), m_dof(0) {
   // Make sure function pointer is valid.
   if (0 == m_func) {
     throw std::logic_error("ChiSq::ChiSq(...): function pointer is NULL");
   }

   // Make sure domain and range have same dimensions.
   if (m_domain->size() != m_range->size()) {
     throw std::runtime_error("ChiSq::ChiSq(...): domain and range do not have same size");
   }

   // Make sure either way of computing reduced chi-square (/ dof or / (dof - 1)) will work correctly.
   if (m_domain->size() <= m_func->getNumParams() + 1) {
     throw std::runtime_error("ChiSq::ChiSq(...) Chi Square is not valid: too few degrees of freedom");
   }

   // Degrees of freedom is the size of the domain, less the number of free parameters.
   m_dof = m_domain->size() - m_func->getNumFreeParams();

   // Mirror the function's parameters in this object's parameters, so that Function's methods will work correctly
   // when called for this object.
   m_func->getParams(m_parameter);
}

double ChiSq::value() const {
   const DataCont_t & x(*m_domain);
   const DataCont_t & y(*m_range);

   double chi_sq = 0.;

   // Compute ChiSq for the current values of the function.
   // ChiSq == sum((y_actual - y_model) ^ 2 / y_model)
   for (DataCont_t::size_type domain_index = 0; domain_index != x.size(); ++domain_index) {
      optimizers::dArg arg(x[domain_index]);
      double func_value = m_func->value(arg);
      double deviation = y[domain_index] - func_value;
      chi_sq += deviation * deviation / func_value;
   }

   return chi_sq;
}

void ChiSq::getFreeDerivs(std::vector<double> & derivs) const {
   const DataCont_t & x(*m_domain);
   const DataCont_t & y(*m_range);

   derivs.assign(m_func->getNumFreeParams(), 0.);

   // Using chain rule on ChiSq:
   // DerivByPar(ChiSq) == sum((1. - (y_actual / y_model) ^ 2) * DerivByPar(y_model))
   // First compute the sum of terms.
   for (DataCont_t::size_type domain_index = 0; domain_index != x.size(); ++domain_index) {
      optimizers::dArg arg(x[domain_index]);
      std::vector<double> func_deriv(derivs.size());

      // Get the functions derivatives wrt free parameters.
      m_func->getFreeDerivs(arg, func_deriv);

      // Precompute the part of the sum which does not depend on which derivative is being taken.
      double ratio = y[domain_index] / m_func->value(arg);
      double deviation_part = 1. - (ratio * ratio);

      // Add terms to output array of derivatives.
      for (std::vector<double>::size_type par_index = 0; par_index != derivs.size(); ++par_index) {
         derivs[par_index] += deviation_part * func_deriv[par_index];
      }
   }

}

std::vector<double>::const_iterator ChiSq::setFreeParamValues_(std::vector<double>::const_iterator it) {
   // Pass modified parameters to the function.
   m_func->setFreeParamValues_(it);
   // Also store the parameters in this object's parameter set, so that other methods from Function such
   // as getFreeParamValues() work correctly .
   return Function::setFreeParamValues_(it);
}

optimizers::Function * ChiSq::clone() const { return new ChiSq(*this); }

double ChiSq::value(optimizers::Arg &) const { return value(); }

double ChiSq::derivByParam(optimizers::Arg &, const std::string & parameter_name) const {
   const DataCont_t & x(*m_domain);
   const DataCont_t & y(*m_range);

   double chi_sq = 0.;

   // Using chain rule on ChiSq:
   // DerivByPar(ChiSq) == sum((1 - (y_actual / y_model) ^ 2) * DerivByPar(y_model))
   for (DataCont_t::size_type domain_index = 0; domain_index != x.size(); ++domain_index) {
      optimizers::dArg arg(x[domain_index]);
      // Compute ratio of actual data to model.
      double ratio = y[domain_index] / m_func->value(arg);

      // Get the function's derivatives wrt the parameter.
      double func_deriv = m_func->derivByParam(arg, parameter_name);

      // Add to the sum term.
      chi_sq += (1 - ratio * ratio) * func_deriv;
   }

   return chi_sq;
}

void ChiSq::getFreeDerivs(optimizers::Arg &, std::vector<double> & derivs) const {
   getFreeDerivs(derivs);
}

}
