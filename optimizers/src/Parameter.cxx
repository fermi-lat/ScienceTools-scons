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

#include "xml/Dom.h"
#include "xml/XmlParser.h"

#include "optimizers/Parameter.h"

namespace {
   DomDocument * createDocument() {
      DomDocument * doc = new DOM_Document();
      *doc = DOM_Document::createDocument();
      return doc;
   }
   DomElement * createElement(DomDocument * doc, const std::string & name) {
      DomElement * elt = new DOM_Element();
      *elt = doc->createElement(name.c_str());
      return elt;
   }
}

namespace optimizers {

void Parameter::setValue(double value) throw(OutOfBounds) {
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

std::pair<double, double> Parameter::getBounds() const {
   std::pair<double, double> my_Bounds(m_minValue, m_maxValue);
   return my_Bounds;
}

void Parameter::extractDomData(const DOM_Element &elt) {
   m_name = xml::Dom::getAttribute(elt, "name");
   m_value = ::atof( xml::Dom::getAttribute(elt, "value").c_str() );
   m_minValue = ::atof( xml::Dom::getAttribute(elt, "min").c_str() );
   m_maxValue = ::atof( xml::Dom::getAttribute(elt, "max").c_str() );
   if (std::string(xml::Dom::getAttribute(elt, "free")) == "true" ||
       std::string(xml::Dom::getAttribute(elt, "free")) == "1" ) {
      m_free = true;
   } else {
      m_free = false;
   }
   m_scale = ::atof( xml::Dom::getAttribute(elt, "scale").c_str() );
}

DomElement Parameter::createDomElement(DomDocument &doc) const {

   DomElement * paramElt = ::createElement(&doc, "parameter");

// Add the appropriate attributes.
   xml::Dom::addAttribute(*paramElt, "name", m_name.c_str());
   xml::Dom::addAttribute(*paramElt, std::string("value"), m_value);
   xml::Dom::addAttribute(*paramElt, std::string("min"), m_minValue);
   xml::Dom::addAttribute(*paramElt, std::string("max"), m_maxValue);
   xml::Dom::addAttribute(*paramElt, std::string("free"), m_free);
   xml::Dom::addAttribute(*paramElt, std::string("scale"), m_scale);

   return *paramElt;
}

} // namespace optimizers
