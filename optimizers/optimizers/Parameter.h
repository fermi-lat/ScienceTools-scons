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

public:

   Parameter() : m_name(""), m_value(0), m_minValue(-HUGE), m_maxValue(HUGE),
                 m_free(true), m_scale(1.), m_error(0) {}

   /// @param name The name of the Parameter
   /// @param value The (scaled) value of the Parameter
   /// @param minValue Parameter value lower bound
   /// @param maxValue Parameter value upper bound
   /// @param isFree true if the Parameter value is allowed to vary in a fit
   /// @param error estimated error on Parameter value.
   Parameter(const std::string & name, double value, double minValue,
             double maxValue, bool isFree=true, double error=0) 
      : m_name(name), m_value(value), m_minValue(minValue), 
        m_maxValue(maxValue), m_free(isFree), m_scale(1.), m_error(error) {}

   Parameter(const std::string & name, double value, bool isFree=true)
      : m_name(name), m_value(value), m_minValue(-HUGE), m_maxValue(HUGE),
        m_free(isFree), m_scale(1.), m_error(0) {}

   ~Parameter() {}

   /// name access
   void setName(const std::string & paramName) {
      m_name = paramName;
   }
   const std::string & getName() const {
      return m_name;
   }
   
   /// value access
   void setValue(double value);
   double getValue() const {
      return m_value;
   }
   
   /// scale access
   void setScale(double scale) {
      m_scale = scale;
   }
   double getScale() const {
      return m_scale;
   }

   /// "true" value access
   void setTrueValue(double trueValue);
   double getTrueValue() const {
      return m_value*m_scale;
   }

   /// bounds access
   void setBounds(double minValue, double maxValue);
   void setBounds(const std::pair<double, double> &boundValues) {
      setBounds(boundValues.first, boundValues.second);
   }
   std::pair<double, double> getBounds() const;

   /// free flag access
   void setFree(bool free) {
      m_free = free;
   }
   bool isFree() const {
      return m_free;
   }

   /// error access
   void setError(double error) {
      m_error = error;
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

private:

   std::string m_name;
   double m_value;
   double m_minValue;
   double m_maxValue;

   /// flag to indicate free or fixed
   bool m_free;

   double m_scale;

   /// estimated error on value
   double m_error;

};

} // namespace optimizers

#endif // optimizers_Parameter_h
