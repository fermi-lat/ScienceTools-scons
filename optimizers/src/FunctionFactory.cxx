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

#include "optimizers/Exception.h"
#include "optimizers/Dom.h"
#include "optimizers/FunctionFactory.h"

// #include "PowerLaw.h"
// #include "Gaussian.h"
// #include "AbsEdge.h"

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

Function *FunctionFactory::create(const std::string &name) throw(Exception) {
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
      errorMessage << "FunctionFactory::readXml: "
                   << "Input xml file, " << xmlFile 
                   << " not parsed successfully.\n";
      throw Exception(errorMessage.str());
   }

   DOM_Element function_library = doc.getDocumentElement();
   Dom::checkTag(function_library, "function_library", 
                 "FunctionFactory::readXml");

// Loop through function child elements, and add each as a Function
// object to the prototype factory.
   std::vector<DOM_Element> funcs;
   Dom::getElements(function_library, "function", funcs);

   std::vector<DOM_Element>::const_iterator funcIt = funcs.begin();
   for ( ; funcIt != funcs.end(); funcIt++) {

// Get the type of this function, which should be an existing 
// (generic) Function in the factory.
      std::string type = xml::Dom::getAttribute(*funcIt, "type");
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
      std::string name = xml::Dom::getAttribute(*funcIt, "name");
      funcObj->setName("name");

// Fetch the parameter elements and set the Parameter data members.
      std::vector<DOM_Element> params;
      Dom::getElements(*funcIt, "parameter", params);
      
      std::vector<DOM_Element>::const_iterator paramIt = params.begin();
      for (; paramIt != params.end(); paramIt++) {
         Parameter parameter;
         Dom::readParamData(*paramIt, parameter);
         funcObj->setParam(parameter);
      }
      addFunc(name, funcObj, false);
   }
   delete parser;
}

} // namespace optimizers
