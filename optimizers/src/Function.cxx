/** 
 * @file Function.cxx
 * @brief Function class implementation
 * @author J. Chiang
 *
 * $Header$
 */

#include <sstream>

#include "xmlBase/Dom.h"

#include "optimizers/Dom.h"
#include "optimizers/Function.h"
#include "optimizers/ParameterNotFound.h"

namespace optimizers {

Function::Function(const std::string & genericName, 
                   unsigned int maxNumParams,
                   const std::string & normParName,
                   const std::string & argType,
                   FuncType funcType)
   : m_genericName(genericName),
     m_maxNumParams(maxNumParams),
     m_normParName(normParName),
     m_argType(argType),
     m_funcType(funcType),
     m_scalingFunction(0) {
}

Function::~Function() {
   delete m_scalingFunction;
}

Function::Function(const Function & other) 
   : m_parameter(other.m_parameter),
     m_genericName(other.m_genericName),
     m_maxNumParams(other.m_maxNumParams),
     m_normParName(other.m_normParName),
     m_argType(other.m_argType),
     m_funcType(other.m_funcType),
     m_scalingFunction(0),
     m_functionName(other.m_functionName) {
   if (other.m_scalingFunction) {
      m_scalingFunction = other.m_scalingFunction->clone();
   }
}

Function & Function::operator=(const Function & rhs) {
   if (this != &rhs) {
      m_parameter = rhs.m_parameter;
      m_genericName = rhs.m_genericName;
      m_maxNumParams = rhs.m_maxNumParams;
      m_normParName = rhs.m_normParName;
      m_argType = rhs.m_argType;
      m_funcType = rhs.m_funcType;
      delete m_scalingFunction;
      m_scalingFunction = 0;
      if (rhs.m_scalingFunction) {
         m_scalingFunction = rhs.m_scalingFunction->clone();
      }
      m_functionName = rhs.m_functionName;
   }
   return *this;
}

double Function::operator()(Arg & xarg) const {
   double my_value(value(xarg));
   if (m_scalingFunction) {
      my_value *= m_scalingFunction->operator()(xarg);
   }
   return my_value;
}

double Function::derivByParam(Arg & xarg,
                              const std::string & paramName) const {
   double my_deriv(derivByParamImp(xarg, paramName));
   if (m_scalingFunction) {
      my_deriv *= m_scalingFunction->operator()(xarg);
   }
   return my_deriv;
}

void Function::setScalingFunction(const Function & scalingFunction) {
   m_scalingFunction = scalingFunction.clone();
}

const Function * Function::scalingFunction() const {
   return m_scalingFunction;
}

void Function::setParam(const Parameter &param) {
   parameter(param.getName()) = param;
}

double Function::getParamValue(const std::string &paramName) const {
   return getParam(paramName).getValue();
}

const Parameter & Function::getParam(const std::string &paramName) const {
   std::vector<Parameter>::const_iterator it = m_parameter.begin();
   for (; it != m_parameter.end(); ++it) {
      if (paramName == it->getName()) {
         return *it;
      }
   }
   throw ParameterNotFound(paramName, getName(), "getParam");
}

void Function::setParamValues(const std::vector<double> &paramVec) {
   if (paramVec.size() != m_parameter.size()) {
      std::ostringstream errorMessage;
      errorMessage << "Function::setParamValues: "
                   << "The input vector size does not match "
                   << "the number of parameters.\n";
      throw Exception(errorMessage.str());
   } else {
      std::vector<double>::const_iterator it = paramVec.begin();
      setParamValues_(it);
   }
}
   
std::vector<double>::const_iterator Function::setParamValues_(
   std::vector<double>::const_iterator it) {
   for (unsigned int i = 0; i < m_parameter.size(); i++) {
      m_parameter[i].setValue(*it++);
   }
   return it;
}

void Function::setParams(const std::vector<Parameter> & params) {
   if (params.size() == m_parameter.size()) {
      m_parameter = params;
   } else {
      throw Exception
         ("Function::setParams: incompatible number of parameters.");
   }
}

void Function::setFreeParamValues(const std::vector<double> &paramVec) {
   if (paramVec.size() != getNumFreeParams()) {
      std::ostringstream errorMessage;
      errorMessage << "Function::setFreeParamValues: "
                   << "The input vector size " << paramVec.size() 
		   << "  does not match " << getNumFreeParams() << " , " 
                   << "the number of free parameters.\n";
      throw Exception(errorMessage.str());
   } else {
      std::vector<double>::const_iterator it = paramVec.begin();
      setFreeParamValues_(it);
   }
}

std::vector<double>::const_iterator Function::setFreeParamValues_(
   std::vector<double>::const_iterator it) {
   for (unsigned int i = 0; i < m_parameter.size(); i++) {
      if (m_parameter[i].isFree()) {
         m_parameter[i].setValue(*it++);
      }
   }
   return it;
}

unsigned int Function::getNumFreeParams() const {
   int j = 0;
   for (unsigned int i = 0; i < m_parameter.size(); i++) {
      j += m_parameter[i].isFree();
   }
   return j;
}

void Function::getFreeParams(std::vector<Parameter> &params) const {
   if (!params.empty()) params.clear();
   
   for (unsigned int i = 0; i < m_parameter.size(); i++) {
      if (m_parameter[i].isFree()) {
         params.push_back(m_parameter[i]);
      }
   }
}

void Function::setParam(const std::string &paramName, 
                        double paramValue) {
   Parameter & par = parameter(paramName);
   par.setValue(paramValue);
}

void Function::addParam(const std::string & paramName,
			double paramValue, 
			bool isFree) {
   try {
      parameter(paramName);
      std::ostringstream errorMessage;
      errorMessage << "Function::addParam:\n"
                   << "This parameter name already exists: "
                   << paramName << "; "
                   << "you can't add another one.\n";
      throw Exception(errorMessage.str());
   } catch (optimizers::ParameterNotFound &) {
      if (m_parameter.size() < m_maxNumParams) {
         Parameter my_param(paramName, paramValue, isFree);
         m_parameter.push_back(my_param);
      } else {
         std::ostringstream errorMessage;
         errorMessage << "Function::addParam: " 
                      << "Can't add parameter " << paramName << ". "
                      << "The parameter list is full at " 
                      << m_maxNumParams << ".\n";
         throw Exception(errorMessage.str());
      }
   }
}

void Function::addParam(const Parameter &param) {
   
   try {
      parameter(param.getName());
      std::ostringstream errorMessage;
      errorMessage << "Function::addParam: "
                   << "This parameter name already exists: "
                   << param.getName() << "; "
                   << "you can't add another one.\n";
      throw Exception(errorMessage.str());
   } catch (optimizers::ParameterNotFound &) {
      if (m_parameter.size() < m_maxNumParams) {
         m_parameter.push_back(param);
      } else {
         std::ostringstream errorMessage;
         errorMessage << "Can't add parameter " << param.getName() << "; "
                      << "the parameter list is full at " 
                      << m_maxNumParams << ".\n";
         throw Exception(errorMessage.str());
      }
   }
}

void Function::fetchParamValues(std::vector<double> &values,
                                bool getFree) const {
   if (!values.empty()) values.clear();

   for (unsigned int i = 0; i < m_parameter.size(); i++) {
      if (!getFree || m_parameter[i].isFree()) {
         values.push_back(m_parameter[i].getValue());
      }
   }
}

void Function::fetchParamNames(std::vector<std::string> &names,
                               bool getFree) const {
   if (!names.empty()) names.clear();

   for (unsigned int i = 0; i < m_parameter.size(); i++) {
      if (!getFree || m_parameter[i].isFree()) {
         names.push_back(m_parameter[i].getName());
      }
   }
}

void Function::fetchDerivs(Arg &x, std::vector<double> &derivs, 
                           bool getFree) const {
   if (!derivs.empty()) derivs.clear();

   for (unsigned int i = 0; i < m_parameter.size(); i++) {
      if (!getFree || m_parameter[i].isFree()) {
         derivs.push_back(derivByParam(x, m_parameter[i].getName()));
      }
   }
}

void Function::appendParamDomElements(DOMDocument * doc, DOMNode * node) {
   std::vector<Parameter>::iterator paramIt = m_parameter.begin();
   for ( ; paramIt != m_parameter.end(); ++paramIt) {
      DOMElement * paramElt = paramIt->createDomElement(doc);
      Dom::appendChild(node, paramElt);
   }
}

void Function::setParams(const DOMElement * elt) {
   std::vector<DOMElement *> parElts;
   xmlBase::Dom::getChildrenByTagName(elt, "parameter", parElts);
   for (unsigned int i = 0; i < parElts.size(); i++) {
      std::string name = xmlBase::Dom::getAttribute(parElts[i], "name");
      parameter(name).extractDomData(parElts[i]);
   }
}

Parameter & Function::parameter(const std::string & name) {
   std::vector<Parameter>::iterator it = m_parameter.begin();
   for (; it != m_parameter.end(); ++it) {
      if (it->getName() == name) {
         return *it;
      } 
   }
   throw ParameterNotFound(name, getName(), "parameter(std::string &)");
}

Parameter & Function::normPar() {
   if (m_normParName == "") {
      throw ParameterNotFound("Normalization", getName(), "normPar()");
   }
   return parameter(m_normParName);
}

const Parameter & Function::normPar() const {
   if (m_normParName == "") {
      throw ParameterNotFound("Normalization", getName(), "normPar()");
   }
   return getParam(m_normParName);
}

bool Function::rescale(double factor) {
   if (m_normParName == "") {
      return false;
   }
   Parameter & prefactor(normPar());
   if (!prefactor.isFree()) {
      return false;
   }
   double new_value(prefactor.getValue()*factor);
   prefactor.setValue(new_value);
   return true;
}

void Function::setMaxNumParams(size_t maxNumParams) {
   m_maxNumParams = maxNumParams;
}

} // namespace optimizers
