/**
 * @file Dom.cxx
 * @brief Implementation for XML-handling static functions.
 * @author J. Chiang
 *
 * $Header$
 */

#include <sstream>

#include "xml/XmlParser.h"
#include "xml/Dom.h"
#include <xercesc/dom/DOM_NodeList.hpp>

#include "optimizers/Parameter.h"
#include "optimizers/Dom.h"

namespace optimizers {

void Dom::checkTag(const DOM_Element &element, 
                   const std::string &tagName, 
                   const std::string &callingRoutine) throw(Exception) {
   if (element == DOM_Element()) {
      throw Exception(
         "optimizers::Dom::checkTag: Trying to read tag of DOM_Element().\n");
   }
   std::string myTagName( xml::Dom::transToChar(element.getTagName()) );
   if (myTagName != tagName) {
      std::ostringstream errorMessage;
      errorMessage << callingRoutine << ": "
                   << "Bad tag name "
                   << myTagName << ".\n"
                   << "Expected "
                   << tagName << ".\n";
      throw Exception(errorMessage.str());
   }
}

bool Dom::checkTagName(const DOM_Element &element, 
                       const std::string &tagName) {
   if (element == DOM_Element()) {
      throw Exception(
         "optimizers::Dom::checkTag: Trying to read tag of DOM_Element().\n");
   }
   std::string myTagName( xml::Dom::transToChar(element.getTagName()) );
   return myTagName == tagName;
}

// Retrieve all child elements by tag name.
void Dom::getElements(const DOM_Element &parent, const std::string &tagName,
                      std::vector<DOM_Element> &children) {
   children.clear();
   DOM_Element child = xml::Dom::getFirstChildElement(parent);
   while (child != DOM_Element()) {
      if (Dom::checkTagName(child, tagName)) {
         children.push_back(child);
      }
      child = xml::Dom::getSiblingElement(child);
   }
}

} // namespace optimizers
