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
#include "optimizers/ParameterNotFound.h"

class DOM_Node;
class DOM_Document;

namespace optimizers {

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
 * @authors J. Chiang, P. Nolan, T. Burnett 
 *
 * $Header$
 */

class Function {

public:

   Function() {}

// We need only member-wise copying, so comment this out.
//   Function(const Function&);

   virtual ~Function() {}

   /// Provide a string identifier.
   void setName(const std::string &functionName) 
      {m_functionName = functionName;}

   std::string getName() const {return m_functionName;}

   /// Set the Parameter value and free state.
   virtual void setParam(const std::string &paramName, 
                         double paramValue, bool isFree)
      throw(ParameterNotFound) {setParameter(paramName, paramValue, isFree);}

   /// Set the Parameter value, preserving current free state.
   virtual void setParam(const std::string &paramName, double paramValue) 
      throw(ParameterNotFound) {setParameter(paramName, paramValue);}

   /// Set a Parameter using a Parameter object.
   virtual void setParam(const Parameter &param) throw(ParameterNotFound);

   /// Return the Parameter value by name.
   virtual double getParamValue(const std::string &paramName) const
      throw(ParameterNotFound);

   /// Return the Parameter object by name.
   virtual Parameter getParam(const std::string &paramName) const
      throw(ParameterNotFound);
   
   /// Set the bounds of a Parameter.
   virtual void setParamBounds(const std::string &paramName, double lower,
                               double upper) throw(ParameterNotFound);
   
   /// Set the scale of a Parameter.
   virtual void setParamScale(const std::string &paramName, double scale)
      throw(ParameterNotFound);

   /// Set the true value of the Parameter, regardless of its scale.
   virtual void setParamTrueValue(const std::string &paramName, 
                                  double paramValue) throw(ParameterNotFound);

   /// Return the total number of Parameters.
   unsigned int getNumParams() const {return m_parameter.size();}

   /// Set the values of all Parameters at once, assuming the order is
   /// known.
   void setParamValues(const std::vector<double> &paramVec)
      throw(Exception);

   /// Do a bit of name mangling to allow for inheritance of setParamValues
   virtual std::vector<double>::const_iterator setParamValues_(
      std::vector<double>::const_iterator);

   /// Set all the Parameters using a vector of Parameter objects.
   virtual void setParams(std::vector<Parameter> &params) 
      throw(Exception);

   /// Get a vector of the Parameter names.
   void getParamNames(std::vector<std::string> &names) const
      {fetchParamNames(names, false);}

   /// Get a vector of the Parameter values.
   void getParamValues(std::vector<double> &values) const
      {fetchParamValues(values, false);}

   /// Get a vector of the Parameter objects.
   void getParams(std::vector<Parameter> &params) const
      {params = m_parameter;}

   /// Return the number of free Parameters.
   unsigned int getNumFreeParams() const;

   /// Set only the free Parameters using a vector of values.
   void setFreeParamValues(const std::vector<double> &paramVec)
      throw(Exception);

   /// Iterator used for composite Functions and Sources. (Note name
   /// mangling here too.)
   virtual std::vector<double>::const_iterator setFreeParamValues_(
      std::vector<double>::const_iterator);

   /// Get the vector of free Parameter names.
   void getFreeParamNames(std::vector<std::string> &names) const
      {fetchParamNames(names, true);}

   /// Get the vector of free Parameter values.
   void getFreeParamValues(std::vector<double> &values) const
      {fetchParamValues(values, true);}

   /// Get the vector of free Parameter objects.
   void getFreeParams(std::vector<Parameter> &) const;

   /// Set the free Parameters all at once using a vector of Parameter
   /// objects.
   virtual void setFreeParams(std::vector<Parameter> &) 
      throw(Exception);
   
   /// Return the Function value.
   virtual double value(Arg &) const = 0;

   /// Function call operator.
   double operator()(Arg &x) const {return value(x);}
   
   /// Function derivative wrt a Parameter, identified by name.
   virtual double derivByParam(Arg &x, 
                               const std::string &paramName) const = 0;
   
   /// Get a vector of all of the derivatives.
   virtual void getDerivs(Arg &x, std::vector<double> &derivs) const
      {fetchDerivs(x, derivs, false);}
   
   /// Get a vector of the derivatives wrt the free Parameters.
   virtual void getFreeDerivs(Arg &x, std::vector<double> &derivs) const
      {fetchDerivs(x, derivs, true);}

   /// Return the integral of function wrt data variable.
   virtual double integral(Arg &, Arg &) const {return 0;}

   /// The clone function, with default.
   virtual Function *clone() const {return 0;}

#ifndef SWIG
   /// These type fields are used by the Composite Function hierarchy
   /// to determine how Function objects may be combined.
   enum FuncType {None, Addend, Factor};

   FuncType funcType() {return m_funcType;}
#endif

   /// The argType must match for Composite Function objects.
   std::string &argType() {return m_argType;}

   /// Return the generic name of the Function. This should be the
   /// same as the class name.
   std::string &genericName() {return m_genericName;}

   /// Append Parameter DOM_Elements to a DOM_Node.
   void appendParamDomElements(DOM_Document &doc, DOM_Node &node);

   /// Set the Parameters from a Function DOM_Element.
   void setParams(const DOM_Element &elt);

protected:

   std::string m_genericName;

   FuncType m_funcType;

   std::string m_argType;

   unsigned int m_maxNumParams;

   std::string m_functionName;

   mutable std::vector<Parameter> m_parameter;

   void setMaxNumParams(int nParams) {m_maxNumParams = nParams;}

   void setParameter(const std::string &paramName, double paramValue, 
                     int isFree = -1) throw(ParameterNotFound);

   //! for subclass constructor use
   void addParam(const std::string &paramName, 
                 double paramValue, bool isFree) throw(Exception);

   void addParam(const std::string &paramName, double paramValue)
      throw(Exception) {addParam(paramName, paramValue, true);}

   void addParam(const Parameter &param) throw(Exception);

   void fetchParamValues(std::vector<double> &values, bool getFree) const;
   void fetchParamNames(std::vector<std::string> &names, bool getFree) const;

   virtual void fetchDerivs(Arg &x ,std::vector<double> &derivs, 
                            bool getFree) const;
};

} // namespace optimizers

#endif // optimizers_Function_h
