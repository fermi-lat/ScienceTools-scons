/** 
 * @file Parameter.cxx
 * @brief Parameter class implementation
 * @author J. Chiang
 *
 * $Header$
 */

#include <cstdlib>

#include <sstream>
#include <string>
#include <vector>

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>

#include "xml/Dom.h"
#include "xml/XmlParser.h"

#include "optimizers/Dom.h"
#include "optimizers/OutOfBounds.h"
#include "optimizers/Parameter.h"

namespace optimizers {

XERCES_CPP_NAMESPACE_USE

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
}

void Parameter::setTrueValue(double trueValue) {
   double value = trueValue/m_scale;
   setValue(value);
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
}

std::pair<double, double> Parameter::getBounds() const {
   std::pair<double, double> my_Bounds(m_minValue, m_maxValue);
   return my_Bounds;
}

void Parameter::extractDomData(const DOMElement * elt) {
   m_name = xml::Dom::getAttribute(elt, "name");
   m_value = std::atof(xml::Dom::getAttribute(elt, "value").c_str());
   m_minValue = std::atof(xml::Dom::getAttribute(elt, "min").c_str());
   m_maxValue = std::atof(xml::Dom::getAttribute(elt, "max").c_str());
   if (std::string(xml::Dom::getAttribute(elt, "free")) == "true" ||
       std::string(xml::Dom::getAttribute(elt, "free")) == "1" ) {
      m_free = true;
   } else {
      m_free = false;
   }
   m_scale = std::atof(xml::Dom::getAttribute(elt, "scale").c_str());
   if (xml::Dom::hasAttribute(elt, "error")) {
      m_error = std::atof(xml::Dom::getAttribute(elt, "error").c_str());
   } else {
      m_error = 0;
   }
}

DOMElement * Parameter::createDomElement(DOMDocument * doc) const {

   DOMElement * paramElt = Dom::createElement(doc, "parameter");

// Add the appropriate attributes.
   xml::Dom::addAttribute(paramElt, "name", m_name.c_str());
   xml::Dom::addAttribute(paramElt, std::string("value"), m_value);
   xml::Dom::addAttribute(paramElt, std::string("min"), m_minValue);
   xml::Dom::addAttribute(paramElt, std::string("max"), m_maxValue);
   xml::Dom::addAttribute(paramElt, std::string("free"), m_free);
   xml::Dom::addAttribute(paramElt, std::string("scale"), m_scale);
   if (m_error > 0) {
      xml::Dom::addAttribute(paramElt, std::string("error"), m_error);
   }

   return paramElt;
}

} // namespace optimizers
