/** 
 * @file FunctionFactory.cxx 
 * @brief Use the prototype pattern for supplying customized Function
 * objects, the parameters of which are specified in an xml file.
 * 
 * @author J. Chiang
 *
 * $Header$
 */

#include <iostream>
#include <sstream>

#include "xml/XmlParser.h"
#include "xml/Dom.h"
#include <xercesc/dom/DOM_Element.hpp>
#include <xercesc/dom/DOM_NodeList.hpp>

#include "optimizers/FunctionFactory.h"
#include "optimizers/Exception.h"

// #include "PowerLaw.h"
// #include "Gaussian.h"
// #include "AbsEdge.h"

namespace {

/// The functionality of this routine is largely already provided by
/// the XML parser, but for cases where more than one tag name is
/// possible, this provides a check against expectations.
void checkTag(DOM_Element &element, const std::string &tagName, 
              const std::string callingRoutine="") 
   throw(optimizers::Exception) {
   if (element == DOM_Element()) {
      throw optimizers::Exception(
         "::checkTag: Trying to read tag of DOM_Element().\n");
   }
   std::string myTagName( xml::Dom::transToChar(element.getTagName()) );
   if (myTagName != tagName) {
      std::ostringstream errorMessage;
      errorMessage << callingRoutine << ": "
                   << "Bad tag name "
                   << myTagName << ".\n"
                   << "Expected "
                   << tagName << ".\n";
      throw optimizers::Exception(errorMessage.str());
   }
}

} // unnamed namespace

namespace optimizers {

FunctionFactory::FunctionFactory() {
// Clients should be responsible for adding these.
//    addFunc("PowerLaw", new PowerLaw(), false);
//    addFunc("Gaussian", new Gaussian(), false);
//    addFunc("AbsEdge", new AbsEdge(), false);
}

FunctionFactory::~FunctionFactory() {
   std::map<std::string, Function *>::iterator it = m_prototypes.begin();
   for (; it != m_prototypes.end(); it++)
      delete it->second;
}

void FunctionFactory::addFunc(const std::string &name, 
                              optimizers::Function* func, 
                              bool fromClone) throw(Exception) {
   if (!m_prototypes.count(name)) {
      if (fromClone) {
         m_prototypes[name] = func->clone();
      } else {
         m_prototypes[name] = func;
      }
   } else {
      std::ostringstream errorMessage;
      errorMessage << "FunctionFactory::addFunc: A Function named "
                   << name << " already exists!\n";
      throw Exception(errorMessage.str());
   }
}

Function *FunctionFactory::create(const std::string &name) 
   throw(Exception) {
   if (!m_prototypes.count(name)) {
      std::ostringstream errorMessage;
      errorMessage << "FunctionFactory::create: "
                   << "Cannot create Function named "
                   << name << ".\n";
      throw Exception(errorMessage.str());
   }
   return m_prototypes[name]->clone();
}

void FunctionFactory::listFunctions() {
   std::cout << "FunctionFactory Functions: " << std::endl;
   std::map<std::string, Function *>::const_iterator 
      it = m_prototypes.begin();
   for (; it != m_prototypes.end(); it++)
      std::cout << it->first << std::endl;
}

void FunctionFactory::readXml(const std::string &xmlFile) throw(Exception) {
   xml::XmlParser *parser = new xml::XmlParser();

   DOM_Document doc = parser->parse(xmlFile.c_str());

   if (doc == 0) { // xml file not parsed successfully
      std::ostringstream errorMessage;
      errorMessage << "FunctionFactory::FunctionFactory: "
                   << "Input xml file, " << xmlFile 
                   << " not parsed successfully.\n";
      throw Exception(errorMessage.str());
   }

   DOM_Element function_library = doc.getDocumentElement();
   ::checkTag(function_library, "function_library", 
              "FunctionFactory::readXml");

// Loop through child elements, and add each as a Function object to
// the prototype factory.
   DOM_Element func = xml::Dom::getFirstChildElement(function_library);
   while (func != DOM_Element()) {
      ::checkTag(func, "function", "FunctionFactory::readXml");

// Get the type of this function, which should be an existing 
// (generic) Function in the factory.
      std::string type = xml::Dom::getAttribute(func, "type");
      Function *funcObj;
      try {
         funcObj = create(type);
      } catch (Exception &eObj) {
         std::cerr << "FunctionFactory::readXml: "
                   << "Failed to create Function object "
                   << type << std::endl;
         throw;
      }

// Set the name of this function prototype.
      std::string name = xml::Dom::getAttribute(func, "name");
      funcObj->setName("name");

// Loop over parameter elements, read in their attributes, and set the
// Parameter data members.
      DOM_Element param = xml::Dom::getFirstChildElement(func);
      while (param != DOM_Element()) {
         ::checkTag(param, "parameter", "FunctionFactory::readXml");
         std::string paramName = xml::Dom::getAttribute(param, "name");
         double paramValue = 
            ::atof( xml::Dom::getAttribute(param, "value").c_str() );
         double paramMin = 
            ::atof( xml::Dom::getAttribute(param, "min").c_str() );
         double paramMax = 
            ::atof( xml::Dom::getAttribute(param, "max").c_str() );
         bool isFree;
         if (std::string(xml::Dom::getAttribute(param, "free")) == "true")
            isFree = true;
         double paramScale = 
            ::atof( xml::Dom::getAttribute(param, "scale").c_str() );
         Parameter parameter(paramName, paramValue, paramMin, paramMax,
                             isFree);
         parameter.setScale(paramScale);
         funcObj->setParam(parameter);
         param = xml::Dom::getSiblingElement(param);
      }
      addFunc(name, funcObj, false);
      func = xml::Dom::getSiblingElement(func);
   }
}

} // namespace optimizers
