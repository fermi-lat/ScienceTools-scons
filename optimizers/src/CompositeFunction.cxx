/** 
 * @file CompositeFunction.cxx
 * @brief CompositeFunction class implementation
 * @author J. Chiang
 *
 * $Header$
 */

#include <vector>
#include <string>
#include <cmath>
#include <cassert>
#include "optimizers/CompositeFunction.h"

namespace optimizers {

CompositeFunction::CompositeFunction(Function & a, Function & b) 
   : Function("CompositeFunction", a.getNumParams()+b.getNumParams(), 
              "", a.argType(), a.funcType()),
     m_a(a.clone()), m_b(b.clone()){
   if (a.argType() != b.argType()) {
      std::ostringstream message;
      message << "CompositeFunction:\n"
              << "Type mismatch: "
              << a.argType() << " vs "
              << b.argType();
      throw std::runtime_error(message.str());
   }
}

CompositeFunction::CompositeFunction(const CompositeFunction &rhs) 
   : Function(rhs), m_a(rhs.m_a->clone()), m_b(rhs.m_b->clone()) {
   syncParams();
}

void CompositeFunction::setParam(const Parameter &param, 
                                 const std::string &funcName) {
   assert(funcName == m_a->getName() || funcName == m_b->getName());

   if (m_a->getName() == funcName) {
      m_a->setParam(param);
   } else {
      m_b->setParam(param);
   }
   syncParams();
}

const Parameter & 
CompositeFunction::getParam(const std::string & paramName,
                            const std::string & funcName) const {
   assert(funcName == m_a->getName() || funcName == m_b->getName());

   if (m_a->getName() == funcName) {
      return m_a->getParam(paramName);
   } else {
      return m_b->getParam(paramName);
   }
}

void CompositeFunction::syncParams() {
   m_parameter.clear();
   std::vector<Parameter> params;

   m_a->getParams(params);
   for (size_t i(0); i < params.size(); i++) {
      m_parameter.push_back(params[i]);
   }

   m_b->getParams(params);
   for (size_t i(0); i < params.size(); i++) {
      m_parameter.push_back(params[i]);
   }
}

} // namespace optimizers
