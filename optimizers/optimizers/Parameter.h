/** 
 * @file Parameter.h
 * @brief Declaration of Parameter and OutOfBounds classes
 * @author J. Chiang
 *
 * $Header$
 */

#ifndef optimizers_Parameter_h
#define optimizers_Parameter_h

#include <cmath>
#include <vector>
#include <string>
#include <sstream>

#include "optimizers/OutOfBounds.h"

#include "xml/Dom.h"

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
 * @todo Need to check optimizer implementations for unbounded
 * parameters.  Does one simply set bounds at extremal (HUGE) values?
 * This may be a dangerous default behavior since some optimizers like
 * to evaluate at the bounds. Or can one simply set a flag to indicate
 * an unbounded parameter?
 *
 * @authors J. Chiang
 *    
 * $Header$ 
 */

class Parameter {

public:

   Parameter() {init(std::string(""), 0., -HUGE, HUGE, true);}

   /**
    * @param paramName The name of the Parameter
    * @param paramValue The (scaled) value of the Parameter
    * @param minValue Parameter value lower bound
    * @param maxValue Parameter value upper bound
    * @param isFree true if the Parameter value is allowed to vary in a fit
    * @param error estimated error on Parameter value.
    */
   Parameter(const std::string &paramName, double paramValue, 
             double minValue, double maxValue, bool isFree = true, 
             double error=0)
      {init(paramName, paramValue, minValue, maxValue, isFree, error);}

   Parameter(const std::string &paramName, double paramValue, 
             bool isFree = true)
      {init(paramName, paramValue, -HUGE, HUGE, isFree, 0);}

// need only member-wise copying
//   Parameter(const Parameter&);

   ~Parameter() {}

   /// name access
   void setName(const std::string &paramName) {m_name = paramName;}
   std::string getName() const {return m_name;}
   
   /// value access
   void setValue(double value) throw(OutOfBounds);
   double getValue() const {return m_value;}

   /**
    * @brief Scale access.
    *
    * The true value of the Parameter is used in the Function
    * calculation.  Only the (apparent) value is intended to
    * accessible through the value accessor methods of the Function
    * class.
    */
   void setScale(double scale) {m_scale = scale;}
   double getScale() const {return m_scale;}

   /// "true" value access
   void setTrueValue(double trueValue) throw(OutOfBounds);
   double getTrueValue() const {return m_value*m_scale;}

   /// bounds access
   void setBounds(double minValue, double maxValue) throw(OutOfBounds);
   void setBounds(const std::pair<double, double> &boundValues) 
      throw(OutOfBounds) {setBounds(boundValues.first, boundValues.second);}
   std::pair<double, double> getBounds() const;

   /// free flag access
   void setFree(bool free) {m_free = free;}
   bool isFree() const {return m_free;}

   /// error access
   void setError(double error) {m_error = error;}
   double error() const {return m_error;}

#ifndef SWIG
   /// Extract data from an xml parameter element defined using the
   /// FunctionModels.dtd.
   void extractDomData(const DOMElement * elt);

   /// Add a parameter DomElement that contains the current data
   /// member values.
   DOMElement * createDomElement(DOMDocument * doc) const;
#endif

private:

   /// set all the Parameter values (with default scaling)
   void init(const std::string &paramName, double paramValue, 
             double minValue, double maxValue, bool isFree = true,
             double error=0) {
      m_name = paramName; 
      m_value = paramValue;
      m_minValue = minValue;
      m_maxValue = maxValue;
      m_free = isFree;
      m_scale = 1;
      m_error = error;
   }

   /// parameter name
   std::string m_name;

   /// its value
   double m_value;

   /// its scale factor
   double m_scale;

   /// lower bound
   double m_minValue;

   /// upper bound
   double m_maxValue;

   /// flag to indicate free or fixed
   bool m_free;

   /// estimated error on value
   double m_error;

};

} // namespace optimizers

#endif // optimizers_Parameter_h
