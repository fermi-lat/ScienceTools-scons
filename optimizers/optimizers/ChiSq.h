/** 
 * @file ChiSq.h
 * @brief ChiSq class declaration
 * @author James Peachey
 *
 * $Header$
 */

#ifndef optimizers_ChiSq_h
#define optimizers_ChiSq_h

#include <vector>

#include "optimizers/Statistic.h"

namespace optimizers {
   class Arg;
   class Function;

/** 
 * @class ChiSq
 *
 * @brief Standard ChiSq statistic (not reduced ChiSq) for arbitrary function
 *
 * @author James Peachey
 *    
 * $Header$
 */
    
class ChiSq : public Statistic {
public:
   typedef std::vector<double> DataCont_t;

   /**
    * @brief Create a chi squared statistic for the given data set.
    *
    * @param domain The data domain for which to compute the statistic.
    *
    * @param range The data range for which to compute the statistic.
    *
    * @param func Pointer to the function used to compute the model values for the given domain.
   */
   ChiSq(const DataCont_t & domain, const DataCont_t & range, optimizers::Function * func);

   virtual double value() const;

   virtual void getFreeDerivs(std::vector<double> & derivs) const;

   virtual std::vector<double>::const_iterator setFreeParamValues_(std::vector<double>::const_iterator it);

   virtual optimizers::Function * clone() const;

   virtual unsigned long dof() const { return m_dof; }

protected:
   virtual double value(optimizers::Arg & x) const;

   virtual double derivByParam(optimizers::Arg & x, const std::string & parameter_name) const;

   virtual void getFreeDerivs(optimizers::Arg &, std::vector<double> & derivs) const;

private:
   const DataCont_t * m_domain;
   const DataCont_t * m_range;
   optimizers::Function * m_func;
   unsigned long m_dof;

};

} // namespace optimizers

#endif // optimizers_ChiSq_h
