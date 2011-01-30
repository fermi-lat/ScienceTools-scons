/** 
 * @file Parameter.cxx
 * @brief Parameter class implementation
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>

#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>

#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"

#include "optimizers/dArg.h"
#include "optimizers/Dom.h"
#include "optimizers/Function.h"
#include "optimizers/OutOfBounds.h"
#include "optimizers/Parameter.h"

namespace optimizers {

XERCES_CPP_NAMESPACE_USE

Parameter::Parameter(const Parameter & other) 
   : m_name(other.m_name),
     m_value(other.m_value),
     m_minValue(other.m_minValue),
     m_maxValue(other.m_maxValue),
     m_free(other.m_free),
     m_scale(other.m_scale),
     m_error(other.m_error),
     m_alwaysFixed(other.m_alwaysFixed) {
   m_par_ref = other.m_par_ref;
   m_log_prior = other.m_log_prior;
}

Parameter & Parameter::operator=(const Parameter & rhs) {
   if (this == &rhs) {
      return *this;
   }
   m_name = rhs.m_name;
   m_value = rhs.m_value;
   m_minValue = rhs.m_minValue;
   m_maxValue = rhs.m_maxValue;
   m_free = rhs.m_free;
   m_scale = rhs.m_scale;
   m_error = rhs.m_error;
   m_alwaysFixed = rhs.m_alwaysFixed; 
   m_par_ref = rhs.m_par_ref;
   m_log_prior = rhs.m_log_prior;
   return *this;
}

void Parameter::setValue(double value) {
   static double tol(1e-8);
   if (m_minValue != 0  && fabs((value - m_minValue)/m_minValue) < tol) {
      m_value = m_minValue;
   } else if (m_maxValue != 0 && fabs((value - m_maxValue)/m_maxValue) < tol) {
      m_value = m_maxValue;
   } else if (value >= m_minValue && value <= m_maxValue) {
      m_value = value;
   } else {
      throw OutOfBounds(
         "Attempt to set the value outside of existing bounds.", 
         value, m_minValue, m_maxValue, 
         static_cast<int>(OutOfBounds::VALUE_ERROR));
   }
   if (m_par_ref) {
      m_par_ref->setValue(value);
   }
}

void Parameter::setTrueValue(double trueValue) {
   double value = trueValue/m_scale;
   setValue(value);
   if (m_par_ref) {
      m_par_ref->setValue(value);
   }
}

void Parameter::setBounds(double minValue, double maxValue) {
   if (m_value >= minValue && m_value <= maxValue) {
      m_minValue = minValue;
      m_maxValue = maxValue;
   } else {
      throw OutOfBounds(
         "Attempt to set bounds that exclude the existing value.", 
         m_value, minValue, maxValue, 
         static_cast<int>(OutOfBounds::BOUNDS_ERROR));
   }
   if (m_par_ref) {
      m_par_ref->setBounds(minValue, maxValue);
   }
}

std::pair<double, double> Parameter::getBounds() const {
   std::pair<double, double> my_Bounds(m_minValue, m_maxValue);
   return my_Bounds;
}

void Parameter::extractDomData(const DOMElement * elt) {
   m_name = xmlBase::Dom::getAttribute(elt, "name");
   m_value = std::atof(xmlBase::Dom::getAttribute(elt, "value").c_str());
   m_minValue = std::atof(xmlBase::Dom::getAttribute(elt, "min").c_str());
   m_maxValue = std::atof(xmlBase::Dom::getAttribute(elt, "max").c_str());
   if (m_value < m_minValue || m_value > m_maxValue) {
      std::ostringstream message;
      message << "Parameter::extractDomData:\n"
              << "In the XML description of parameter "<< m_name << ", "
              << "An attempt has been made to set the parameter value "
              << "outside of the specified bounds.";
      throw std::out_of_range(message.str());
   }
   if (std::string(xmlBase::Dom::getAttribute(elt, "free")) == "true" ||
       std::string(xmlBase::Dom::getAttribute(elt, "free")) == "1" ) {
      m_free = true;
   } else {
      m_free = false;
   }
   m_scale = std::atof(xmlBase::Dom::getAttribute(elt, "scale").c_str());
   if (xmlBase::Dom::hasAttribute(elt, "error")) {
      m_error = std::atof(xmlBase::Dom::getAttribute(elt, "error").c_str());
   } else {
      m_error = 0;
   }
   if (m_par_ref) {
      m_par_ref->extractDomData(elt);
   }
}

DOMElement * Parameter::createDomElement(DOMDocument * doc) const {

   DOMElement * paramElt = Dom::createElement(doc, "parameter");

// Add the appropriate attributes.
   xmlBase::Dom::addAttribute(paramElt, "name", m_name.c_str());
   xmlBase::Dom::addAttribute(paramElt, std::string("value"), m_value, 10);
   xmlBase::Dom::addAttribute(paramElt, std::string("min"), m_minValue, 10);
   xmlBase::Dom::addAttribute(paramElt, std::string("max"), m_maxValue, 10);
   xmlBase::Dom::addAttribute(paramElt, std::string("free"), m_free);
   xmlBase::Dom::addAttribute(paramElt, std::string("scale"), m_scale, 10);
   if (m_error > 0) {
      xmlBase::Dom::addAttribute(paramElt, std::string("error"), m_error, 10);
   }

   return paramElt;
}

void Parameter::setPrior(Function & log_prior) {
   m_log_prior = &log_prior;
}

double Parameter::log_prior_value() const {
   if (!m_log_prior) {
      return 0;
   }
   dArg x(m_value);
   return m_log_prior->operator()(x);
}

double Parameter::log_prior_deriv() const {
   if (!m_log_prior) {
      return 0;
   }
   dArg x(m_value);
   return m_log_prior->derivative(x);
}

} // namespace optimizers
