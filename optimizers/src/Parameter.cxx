/** 
 * @file Parameter.cxx
 * @brief Parameter class implementation
 * @author J. Chiang
 *
 * $Header$
 */

#include <vector>
#include <string>
#include <sstream>

#include "xml/XmlParser.h"
#include "xml/Dom.h"
#include <xercesc/dom/DOMString.hpp>
#include <xercesc/dom/DOM_Element.hpp>
#include <xercesc/dom/DOM_Document.hpp>
#include <xercesc/dom/DOM_NodeList.hpp>

#include "optimizers/Parameter.h"

namespace optimizers {

void Parameter::setValue(double value) throw(OutOfBounds) {
   if (value >= m_minValue && value <= m_maxValue) {
      m_value = value;
   } else {
      throw OutOfBounds(
         "Attempt to set the value outside of existing bounds.", 
         value, m_minValue, m_maxValue, 
         static_cast<int>(OutOfBounds::VALUE_ERROR));
   }
}

void Parameter::setTrueValue(double trueValue) throw(OutOfBounds) {
   double value = trueValue/m_scale;
   setValue(value);
}

void Parameter::setBounds(double minValue, double maxValue) 
   throw(OutOfBounds) {
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

// return bounds as a pair
std::pair<double, double> Parameter::getBounds() const {
   std::pair<double, double> my_Bounds(m_minValue, m_maxValue);
   return my_Bounds;
}

void Parameter::extractDomData(const DOM_Element &elt) {
   m_name = xml::Dom::getAttribute(elt, "name");
   m_value = ::atof( xml::Dom::getAttribute(elt, "value").c_str() );
   m_minValue = ::atof( xml::Dom::getAttribute(elt, "min").c_str() );
   m_maxValue = ::atof( xml::Dom::getAttribute(elt, "max").c_str() );
   if (std::string(xml::Dom::getAttribute(elt, "free")) == "true") {
      m_free = true;
   } else {
      m_free = false;
   }
   m_scale = ::atof( xml::Dom::getAttribute(elt, "scale").c_str() );
}

DOM_Element Parameter::createDomElement(DOM_Document &doc) const {

   DOM_Element paramElt = doc.createElement("parameter");

// Add the appropriate attributes.
   paramElt.setAttribute(DOMString("name"), DOMString(m_name.c_str()));

   std::ostringstream value;
   value << m_value;
   paramElt.setAttribute(DOMString("value"), DOMString(value.str().c_str()));

   std::ostringstream minValue;
   minValue << m_minValue;
   paramElt.setAttribute(DOMString("min"), DOMString(minValue.str().c_str()));

   std::ostringstream maxValue;
   maxValue << m_maxValue;
   paramElt.setAttribute(DOMString("max"), DOMString(maxValue.str().c_str()));

   std::ostringstream free;
   free << m_free;
   paramElt.setAttribute(DOMString("free"), DOMString(free.str().c_str()));

   std::ostringstream scale;
   scale << m_scale;
   paramElt.setAttribute(DOMString("scale"), DOMString(scale.str().c_str()));

   return paramElt;
}

} // namespace optimizers
