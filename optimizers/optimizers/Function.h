/** 
 * @file Function.h
 * @brief Declaration of Function class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Function_h
#define optimizers_Function_h

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "optimizers/Parameter.h"

namespace optimizers {

#ifndef SWIG
using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;
using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
using XERCES_CPP_NAMESPACE_QUALIFIER DOMNode;
#endif // SWIG

class Arg;

/** 
 * @class Function
 *
 * @brief Base class for Science Tools Functions, such as spectral models, 
 * fit statistics, etc..
 *
 * The implementation is based on Hippodraw's FunctionBase class.
 *
 * This class uses the Parameter and Arg classes.
 *
 * @authors J. Chiang, P. Nolan
 *
 * $Header$
 */

class Function {

   friend class FunctionTest;

public:

   Function() {}

   virtual ~Function() {}

   /// Provide a string identifier.
   void setName(const std::string & functionName) {
      m_functionName = functionName;
   }

   const std::string & getName() const {
      return m_functionName;
   }

   /// Set the Parameter value
   virtual void setParam(const std::string &paramName, double paramValue);

   /// Set a Parameter using a Parameter object.
   virtual void setParam(const Parameter &param);

   /// Return the Parameter value by name.
   virtual double getParamValue(const std::string &paramName) const;

   /// Return the Parameter object by name.
   virtual const Parameter & getParam(const std::string &paramName) const;

   /// Unsafe version.
   Parameter & parameter(const std::string & name);
   
   /// Return the total number of Parameters.
   unsigned int getNumParams() const {
      return m_parameter.size();
   }

   /// Set the values of all Parameters at once, assuming the order is known.
   void setParamValues(const std::vector<double> &paramVec);

   /// Do a bit of name mangling to allow for inheritance of setParamValues
   virtual std::vector<double>::const_iterator setParamValues_(
      std::vector<double>::const_iterator);

   /// Set all the Parameters using a vector of Parameter objects.
   virtual void setParams(std::vector<Parameter> &params);

   /// Get a vector of the Parameter names.
   void getParamNames(std::vector<std::string> &names) const {
      fetchParamNames(names, false);
   }

   /// Get a vector of the Parameter values.
   void getParamValues(std::vector<double> &values) const {
      fetchParamValues(values, false);
   }

   /// Get a vector of the Parameter objects.
   void getParams(std::vector<Parameter> &params) const {
      params = m_parameter;
   }

   /// Return the number of free Parameters.
   unsigned int getNumFreeParams() const;

   /// Set only the free Parameters using a vector of values.
   void setFreeParamValues(const std::vector<double> &paramVec);

   /// Iterator used for composite Functions and Sources. (Note name
   /// mangling here too.)
   virtual std::vector<double>::const_iterator setFreeParamValues_(
      std::vector<double>::const_iterator);

   /// Get the vector of free Parameter names.
   void getFreeParamNames(std::vector<std::string> &names) const {
      fetchParamNames(names, true);
   }

   /// Get the vector of free Parameter values.
   void getFreeParamValues(std::vector<double> &values) const {
      fetchParamValues(values, true);
   }

   /// Get the vector of free Parameter objects.
   void getFreeParams(std::vector<Parameter> &) const;

   /// Return the Function value.
   virtual double value(Arg &) const = 0;

   /// Function call operator.
   double operator()(Arg &x) const {
      return value(x);
   }
   
   /// Function derivative wrt a Parameter, identified by name.
   virtual double derivByParam(Arg &x, 
                               const std::string &paramName) const = 0;
   
   /// Get a vector of all of the derivatives.
   virtual void getDerivs(Arg &x, std::vector<double> &derivs) const {
      fetchDerivs(x, derivs, false);
   }
   
   /// Get a vector of the derivatives wrt the free Parameters.
   virtual void getFreeDerivs(Arg &x, std::vector<double> &derivs) const {
      fetchDerivs(x, derivs, true);
   }

   /// Return the integral of function wrt data variable.
   virtual double integral(Arg &, Arg &) const {
      return 0;
   }

   /// The clone function, with default.
   virtual Function * clone() const {
      return 0;
   }

#ifndef SWIG
   /// These type fields are used by the Composite Function hierarchy
   /// to determine how Function objects may be combined.
   enum FuncType {None, Addend, Factor};

   FuncType funcType() {
      return m_funcType;
   }
#endif // SWIG

   /// The argType must match for Composite Function objects.
   const std::string & argType() const {
      return m_argType;
   }

   /// Return the generic name of the Function. This should be the
   /// same as the class name.
   const std::string & genericName() const {
      return m_genericName;
   }

#ifndef SWIG
   /// Append Parameter DOMElements to a DOMNode.
   void appendParamDomElements(DOMDocument * doc, DOMNode * node);

   /// Set the Parameters from a Function DOM_Element.
   void setParams(const DOMElement * elt);
#endif // SWIG

protected:

   std::string m_genericName;

   FuncType m_funcType;

   std::string m_argType;

   unsigned int m_maxNumParams;

   std::string m_functionName;

   std::vector<Parameter> m_parameter;

   void setMaxNumParams(int nParams) {
      m_maxNumParams = nParams;
   }

   /// for subclass constructor use
   void addParam(const std::string &paramName, 
                 double paramValue, bool isFree=true);

   void addParam(const Parameter &param);

   void fetchParamValues(std::vector<double> &values, bool getFree) const;
   void fetchParamNames(std::vector<std::string> &names, bool getFree) const;

   virtual void fetchDerivs(Arg &x ,std::vector<double> &derivs, 
                            bool getFree) const;

   void setParamAlwaysFixed(const std::string & name) {
      parameter(name).m_alwaysFixed = true;
      parameter(name).setFree(false);
   }

};

} // namespace optimizers

#endif // optimizers_Function_h
