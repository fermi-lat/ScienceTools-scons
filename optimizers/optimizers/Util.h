/**
 * @file Util.h
 * @brief Declaration of utilities for optimizers package
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Util_h
#define optimizers_Util_h

namespace optimizers {

/**
 * @class FunctorBase
 * @brief Base class for functor to be fed to static functions in Util.
 */
class FunctorBase {
public:
   FunctorBase() {}
   virtual double operator()(double x) const = 0;
};

/**
 * @class Functor
 * @brief Concrete FunctorBase implementation that is instantiated with
 * an arbitrary function object class in order to provide a transparent
 * wrapper for its functor behavior.
 */
template<typename func>
class Functor : public FunctorBase {
public:
   Functor(const func & f) : FunctorBase(), m_f(f) {}
   virtual double operator()(double x) const {
      return m_f(x);
   }
private:
   const func & m_f;
};

/**
 * @class Util
 * @brief Static utility functions 
 */

class Util {
public:
   static double numDeriv(FunctorBase & f, double x, double h, double & err);
};

} // namespace optimizers

#endif // optimizers_Util_h
