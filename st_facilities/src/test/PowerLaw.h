/**
 * @file PowerLaw.h
 *
 * $Header$
 */

#include <cmath>
#include <cstdlib>

class PowerLaw {

public:

   PowerLaw(double prefactor, double index) : m_prefactor(prefactor),
                                              m_index(index) {}

   double operator()(double x) const {
      return m_prefactor*std::pow(x, m_index);
   }

private:

   double m_prefactor;
   double m_index;

};
