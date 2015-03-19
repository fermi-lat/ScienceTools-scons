/** 
 * @file Function.h
 * @brief Declaration of Function class
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Function_h
#define optimizers_Function_h

#include <vector>
#include <stdexcept>
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
 */

class Function {

   friend class FunctionTest;

public:

#ifndef SWIG
   /// These type fields are used by the Composite Function hierarchy
   /// to determine how Function objects may be combined.
   enum FuncType {None, Addend, Factor};
#endif // SWIG

   Function(const std::string & genericName,
            unsigned int maxNumParams,
            const std::string & normParName,
            const std::string & argType="dArg",
            FuncType funcType=Addend);

   virtual ~Function();

   Function(const Function & other);

   Function & operator=(const Function & rhs);

   /// Function call operator.  Uses template method so non-virtual.
   double operator()(Arg & xarg) const;
   
   /// Function derivative wrt a Parameter.  Uses template method so
   /// non-virtual.
   double derivByParam(Arg & xarg, 
                       const std::string & paramName) const;

   void setScalingFunction(const Function & scalingFunction);

   const Function * scalingFunction() const;
   
   /// Provide a string identifier.
   void setName(const std::string & functionName) {
      m_functionName = functionName;
   }

   const std::string & getName() const {
      return m_functionName;
   }

   /// Set the Parameter value
   virtual void setParam(const std::string & paramName, double paramValue);

   /// Set a Parameter using a Parameter object.
   virtual void setParam(const Parameter & param);

   /// Return the Parameter value by name.
   virtual double getParamValue(const std::string & paramName) const;

   /// Return the Parameter object by name.
   virtual const Parameter & getParam(const std::string & paramName) const;

   /// @return non-const reference to named Parameter.
   virtual Parameter & parameter(const std::string & name);

   /// @return The parameter controlling the overall normalization.
   virtual Parameter & normPar();
   
   virtual const Parameter & normPar() const;
   
   /// Return the total number of Parameters.
   unsigned int getNumParams() const {
      return m_parameter.size();
   }

   /// Set the values of all Parameters.
   void setParamValues(const std::vector<double> & paramVec);

   /// Do a bit of name mangling to allow for inheritance of setParamValues
   virtual std::vector<double>::const_iterator setParamValues_(
      std::vector<double>::const_iterator);

   /// Set all the Parameters using a vector of Parameter objects.
   virtual void setParams(const std::vector<Parameter> & params);

   /// Get a vector of the Parameter names.
   void getParamNames(std::vector<std::string> & names) const {
      fetchParamNames(names, false);
   }

   /// Get a vector of the Parameter values.
   void getParamValues(std::vector<double> & values) const {
      fetchParamValues(values, false);
   }

   /// Get a vector of the Parameter objects.
   void getParams(std::vector<Parameter> & params) const {
      params = m_parameter;
   }

   /// Return the number of free Parameters.
   virtual unsigned int getNumFreeParams() const;

   /// Set only the free Parameters using a vector of values.
   virtual void setFreeParamValues(const std::vector<double> & paramVec);

   /// Iterator used for composite Functions and Sources. (Note name
   /// mangling here too.)
   virtual std::vector<double>::const_iterator setFreeParamValues_(
      std::vector<double>::const_iterator);

   /// Get the vector of free Parameter names.
   void getFreeParamNames(std::vector<std::string> & names) const {
      fetchParamNames(names, true);
   }

   /// Get the vector of free Parameter values.
   void getFreeParamValues(std::vector<double> & values) const {
      fetchParamValues(values, true);
   }

   /// Get the vector of free Parameter objects.
   virtual void getFreeParams(std::vector<Parameter> &) const;

   /// Get a vector of all of the derivatives.
   virtual void getDerivs(Arg & x, std::vector<double> & derivs) const {
      fetchDerivs(x, derivs, false);
   }
   
   /// Get a vector of the derivatives wrt the free Parameters.
   virtual void getFreeDerivs(Arg & x, std::vector<double> & derivs) const {
      fetchDerivs(x, derivs, true);
   }

   /// Return the integral of function wrt data variable.
   virtual double integral(Arg &, Arg &) const {
     throw std::runtime_error("integral method not implemented for "
			      + m_genericName);
     return 0;
   }

   /// Derivative of function wrt data variable.
   virtual double derivative(Arg &) const {
     throw std::runtime_error("derivative method not implemented for "
			      + m_genericName);
     return 0;
   }

   /// The clone function
   virtual Function * clone() const = 0;

#ifndef SWIG
   FuncType funcType() {
      return m_funcType;
   }
#endif // SWIG

   /// The argType must match for Composite Function objects.
   const std::string & argType() const {
      return m_argType;
   }

   /// Return the generic name of the Function.
   const std::string & genericName() const {
      return m_genericName;
   }

   /// @brief Rescale the overall normalization of the model (by adjusting
   /// the appropriate parameter). 
   /// @return Boolean value: true if rescaling was accomplished, false if
   /// the parameter controlling the normalization is fixed.
   /// @param factor Rescaling factor. 
   virtual bool rescale(double factor);

#ifndef SWIG
   /// Append Parameter DOMElements to a DOMNode.
   void appendParamDomElements(DOMDocument * doc, DOMNode * node);

   /// Set the Parameters from a Function DOM_Element.
   void setParams(const DOMElement * elt);
#endif // SWIG

   void setParamAlwaysFixed(const std::string & name) {
      parameter(name).m_alwaysFixed = true;
      parameter(name).setFree(false);
   }

   // const std::vector<double> & xvalues(size_t nx=100) const {
   //    return m_xvalues;
   // }

protected:

   std::vector<Parameter> m_parameter;

   void setMaxNumParams(size_t maxNumParams);

//   mutable std::vector<double> m_xvalues;

   /// Return the Function value.
   virtual double value(Arg &) const = 0;

   virtual double derivByParamImp(Arg & x,
                                  const std::string & paramName) const = 0;

   /// For subclass usage
   void addParam(const std::string & paramName, 
                 double paramValue, bool isFree=true);

   void addParam(const Parameter & param);

   virtual void fetchParamValues(std::vector<double> & values, 
                                 bool getFree) const;

   void fetchParamNames(std::vector<std::string> & names, bool getFree) const;

   virtual void fetchDerivs(Arg & x ,std::vector<double> & derivs, 
                            bool getFree) const;

   void setNormParName(const std::string & normParName);

   void setGenericName(const std::string & genericName);

private:

   std::string m_genericName;
   unsigned int m_maxNumParams;
   std::string m_normParName;
   std::string m_argType;
   FuncType m_funcType;

   Function * m_scalingFunction;

   std::string m_functionName;

};

} // namespace optimizers

#endif // optimizers_Function_h
