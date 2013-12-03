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

   RosenBounded() : m_prefactor(100), m_xmin(-1e30), m_xmax(1e30),
                    m_ymin(-1e30), m_ymax(1e30) {
      init();
   }

   RosenBounded(double prefactor) : m_prefactor(prefactor), 
                                    m_xmin(-1e30), m_xmax(1e30),
                                    m_ymin(-1e30), m_ymax(1e30) {
      init();
   }
      
   virtual double value(Arg &) const;

   virtual double value() const {
      Arg dummy;
      return value(dummy);
   }

   virtual double derivByParam(Arg &, const std::string & paramName) const;

   virtual void getFreeDerivs(std::vector<double>  & derivs) const {
      Arg dummy;
      Function::getFreeDerivs(dummy, derivs);
   }

   void set_xbounds(double xmin, double xmax);
   void set_ybounds(double ymin, double ymax);

protected:

   virtual RosenBounded * clone() const {
      return new RosenBounded(*this);
   }

private:

   double m_prefactor;

   double m_xmin, m_xmax;
   double m_ymin, m_ymax;

   void init();

   void check_bounds(double x, double y) const;

};

} // namespace optimizers

