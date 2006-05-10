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
   TestOptimizer(Statistic & stat, Optimizer & optimizer) 
      : m_stat(stat), m_opt(optimizer) {
      m_stat.getParams(m_params);
   }
   ~TestOptimizer() throw() {
      try {
         m_stat.setParams(m_params);
      } catch (...) {
      }
   }
   void fit(int verbose=0, double tol=1e-5) {
      try {
         m_opt.find_min(verbose, tol);
      } catch (std::exception & eObj) {
         std::cout << eObj.what() << std::endl;
      }
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
   Statistic & m_stat;
   Optimizer & m_opt;
   std::vector<Parameter> m_params;
};

} // namespace optimizer

#endif // optimizers_TestOptimizer_h
