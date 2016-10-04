/**
 * @file RootFinder.h
 * @brief Wrapper for GSL root finder
 * @author M. Wood
 *
 * $Header$
 */

#ifndef st_facilities_RootFinder_h
#define st_facilities_RootFinder_h

// Don't build for containers other than ScienceTools since the gsl
// libraries aren't available.
#ifdef ScienceTools

#include <iostream>
#include <cmath>
#include <stdexcept>
#include <string>

#include "gsl/gsl_roots.h"
#include "gsl/gsl_errno.h"

namespace st_facilities {

/** 
 * @class gsl_function_wrapper
 *
 * @brief Wrapper class for passing functors into GSL.
 *
 */
template< typename Functor >  
class gsl_function_wrapper : public gsl_function {

public:
  gsl_function_wrapper(const Functor& func, double val = 0.0) : 
    m_func(func), m_val(val) {
      function = &gsl_function_wrapper::invoke;
      params=this;
    }

private:

  const Functor&  m_func;
  double          m_val;

  static double invoke(double x, void *params) {
    gsl_function_wrapper* fn = static_cast<gsl_function_wrapper*>(params);
    return fn->m_func(x) - fn->m_val;
  }
};

/**
 * @class RootFinder
 *
 * @brief One-dimensional root finder that uses the Brent method from
 * the GSL library.
 *
 */

class RootFinder {

public:
  template< typename Functor > 
  static double find_root(const Functor& func, 
			  double x_lo, double x_hi, double y_value,
			  double rtol=0.001, double atol=0) {
    
    gsl_root_fsolver * s = gsl_root_fsolver_alloc(gsl_root_fsolver_brent);
    
    gsl_function_wrapper<Functor> F(func, y_value);
    gsl_root_fsolver_set(s, &F, x_lo, x_hi);

    int iter = 0, max_iter = 100;
    int status = GSL_CONTINUE;
    double root = 0.0;

    while(status == GSL_CONTINUE && iter < max_iter) {
      iter++;
      status = gsl_root_fsolver_iterate(s);
      root = gsl_root_fsolver_root(s);
      x_lo = gsl_root_fsolver_x_lower(s);
      x_hi = gsl_root_fsolver_x_upper(s);
      status = gsl_root_test_interval(x_lo, x_hi, atol, rtol);  
    }

    gsl_root_fsolver_free(s);
    return root;
  }
};

}

#endif // ScienceTools
#endif // st_facilities_RootFinder_h
