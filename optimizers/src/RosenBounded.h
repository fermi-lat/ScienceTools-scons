/** 
 * @file RosenBounded.h
 * @brief Declaration for a 2D Rosenbrock objective function
 * @author J. Chiang
 *
 * $Header$
 */

#include "optimizers/Statistic.h"

namespace optimizers {
    
class RosenBounded : public Statistic {

public:

   RosenBounded(double prefactor=100);
      
   virtual double value() const {
      Arg dummy;
      return value(dummy);
   }

   virtual void getFreeDerivs(std::vector<double>  & derivs) const {
      Arg dummy;
      Function::getFreeDerivs(dummy, derivs);
   }

   virtual Function * clone() const {
      return new RosenBounded(*this);
   }


   void set_xbounds(double xmin, double xmax);
   void set_ybounds(double ymin, double ymax);

protected:

   virtual double value(const Arg &) const;

   virtual double derivByParamImp(const Arg &,
                                  const std::string & paramName) const;

private:

   double m_prefactor;

   double m_xmin, m_xmax;
   double m_ymin, m_ymax;

   void check_bounds(double x, double y) const;

};

} // namespace optimizers

