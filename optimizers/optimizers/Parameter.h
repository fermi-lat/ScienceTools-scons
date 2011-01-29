/** 
 * @file Parameter.h
 * @brief Declaration of Parameter classe
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Parameter_h
#define optimizers_Parameter_h

#include <cmath>

#include <sstream>
#include <string>
#include <vector>

#include "xmlBase/Dom.h"

namespace optimizers {

   class Function;

#ifndef SWIG
using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;
#endif

/** 
 * @class Parameter
 *
 * @brief Model parameters are identified by a name with flags to
 * indicate if it's free and with upper and lower bounds.
 *
 * The true value of the Parameter is used in the Function
 * calculation.  Only the (apparent) value is intended to accessible
 * through the value accessor methods of the Function class.
 *
 * @authors J. Chiang
 *    
 * $Header$ 
 */

class Parameter {

   friend class Function;

public:

   Parameter() : m_name(""), m_value(0), m_minValue(-HUGE), m_maxValue(HUGE),
                 m_free(true), m_scale(1.), m_error(0), m_alwaysFixed(false),
                 m_par_ref(0), m_log_prior(0), m_log_prior_deriv(0) {}

   /// @param name The name of the Parameter
   /// @param value The (scaled) value of the Parameter
   /// @param minValue Parameter value lower bound
   /// @param maxValue Parameter value upper bound
   /// @param isFree true if the Parameter value is allowed to vary in a fit
   /// @param error estimated error on Parameter value.
   Parameter(const std::string & name, double value, double minValue,
             double maxValue, bool isFree=true, double error=0) 
      : m_name(name), m_value(value), m_minValue(minValue), 
        m_maxValue(maxValue), m_free(isFree), m_scale(1.), m_error(error),
        m_alwaysFixed(false), m_par_ref(0), m_log_prior(0),
        m_log_prior_deriv(0) {}

   Parameter(const std::string & name, double value, bool isFree=true)
      : m_name(name), m_value(value), m_minValue(-HUGE), m_maxValue(HUGE),
        m_free(isFree), m_scale(1.), m_error(0), m_alwaysFixed(false),
        m_par_ref(0), m_log_prior(0), m_log_prior_deriv(0) {}

   Parameter(const Parameter & other);

   Parameter & operator=(const Parameter & rhs);

   virtual ~Parameter() throw() {}

   /// name access
   virtual void setName(const std::string & paramName) {
      m_name = paramName;
      if (m_par_ref) {
         m_par_ref->setName(paramName);
      }
   }

   const std::string & getName() const {
      return m_name;
   }
   
   /// value access
   virtual void setValue(double value);

   double getValue() const {
      return m_value;
   }
   
   /// scale access
   virtual void setScale(double scale) {
      m_scale = scale;
      if (m_par_ref) {
         m_par_ref->setScale(scale);
      }
   }
   double getScale() const {
      return m_scale;
   }

   /// "true" value access
   virtual void setTrueValue(double trueValue);
   double getTrueValue() const {
      return m_value*m_scale;
   }

   /// bounds access
   virtual void setBounds(double minValue, double maxValue);
   virtual void setBounds(const std::pair<double, double> &boundValues) {
      setBounds(boundValues.first, boundValues.second);
   }
   std::pair<double, double> getBounds() const;

   /// free flag access
   virtual void setFree(bool free) {
      if (m_alwaysFixed) {
         m_free = false;
      } else {
         m_free = free;
      }
      if (m_par_ref) {
         m_par_ref->setFree(free);
      }
   }
   bool isFree() const {
      return m_free;
   }

   virtual void setAlwaysFixed(bool flag) {
      m_alwaysFixed = flag;
      if (m_par_ref) {
         m_par_ref->setAlwaysFixed(flag);
      }
   }
   bool alwaysFixed() const {
      return m_alwaysFixed;
   }

   /// error access
   virtual void setError(double error) {
      m_error = error;
      if (m_par_ref) {
         m_par_ref->setError(error);
      }
   }
   double error() const {
      return m_error;
   }

#ifndef SWIG
   /// Extract data from an xml parameter element defined using the
   /// FunctionModels.dtd.
   void extractDomData(const DOMElement * elt);

   /// Add a parameter DomElement that contains the current data
   /// member values.
   DOMElement * createDomElement(DOMDocument * doc) const;
#endif // SWIG

   void setParRef(Parameter * par) {
      m_par_ref = par;
      m_name = par->m_name;
      m_value = par->m_value;
      m_minValue = par->m_minValue;
      m_maxValue = par->m_maxValue;
      m_free = par->m_free;
      m_scale = par->m_scale;
      m_error = par->m_error;
      m_alwaysFixed = par->m_alwaysFixed;
   }

   double log_prior() const;

   double log_prior_deriv() const;

protected:

   std::string m_name;
   double m_value;
   double m_minValue;
   double m_maxValue;

   /// flag to indicate free or fixed
   bool m_free;

   double m_scale;

   /// estimated error on value
   double m_error;

   /// If true, then m_free is always false and cannot be changed.
   bool m_alwaysFixed;

   /// pointer to underlying Parameter object for use by composite Function
   /// classes. This should not be deleted by this class.
   Parameter * m_par_ref;

   /// Pointers to prior function (assumed to be the log of the 1D
   /// PDF) and its derivative.  These will not be deleted by this
   /// class.
   Function * m_log_prior;

   Function * m_log_prior_deriv;
   
};

} // namespace optimizers

#endif // optimizers_Parameter_h
