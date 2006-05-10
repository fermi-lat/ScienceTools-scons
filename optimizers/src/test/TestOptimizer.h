/**
 * @file TestOptimizer.h
 * @brief Runs the optimizer for a specified Statistic object and 
 * prints the results.
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_TestOptimizer_h
#define optimizers_TestOptimizer_h

#include <iostream>
#include <stdexcept>
#include <vector>

#include "optimizers/Optimizer.h"
#include "optimizers/Parameter.h"
#include "optimizers/Statistic.h"

namespace optimizers {

class TestOptimizer {
public:
   TestOptimizer(Optimizer & optimizer) 
      : m_opt(optimizer), m_stat(m_opt.stat()) {
      m_stat.getParams(m_params);
   }
   ~TestOptimizer() throw() {
      try {
         m_stat.setParams(m_params);
      } catch (...) {
      }
   }
   void run(int verbose=0, double tol=1e-5) {
      try {
         fit(verbose, tol);
         printResults();
      } catch (std::exception & eObj) {
         std::cout << "\n" << eObj.what() << "\n" << std::endl;
      }
   }
   void fit(int verbose=0, double tol=1e-5) {
      m_opt.find_min(verbose, tol);
   }
   void printResults() {
      std::vector<Parameter> params;
      std::vector<double> sigmas;
      m_stat.getParams(params);
      sigmas = m_opt.getUncertainty();
      std::cout << "Model parameters and errors:" << std::endl;
      for (size_t i = 0; i < params.size(); i++) {
         std::cout << i << "  " 
                   << params.at(i).getValue() << "  "
                   << sigmas.at(i) << std::endl;
      }
      std::cout << std::endl;
   }
private:
   Optimizer & m_opt;
   Statistic & m_stat;
   std::vector<Parameter> m_params;
};

} // namespace optimizer

#endif // optimizers_TestOptimizer_h
