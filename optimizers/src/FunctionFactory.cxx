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
#include <fstream>

#include "xml/XmlParser.h"
#include "xml/Dom.h"
#include <xercesc/dom/DOM_Document.hpp>
#include <xercesc/dom/DOM_Element.hpp>
#include <xercesc/dom/DOM_NodeList.hpp>
#include <xercesc/dom/DOM_DOMException.hpp>

#include "optimizers/Exception.h"
#include "optimizers/Dom.h"
#include "optimizers/FunctionFactory.h"

#include "PowerLaw.h"
#include "BrokenPowerLaw.h"
#include "Gaussian.h"
#include "AbsEdge.h"
#include "ConstantValue.h"

namespace optimizers {

FunctionFactory::FunctionFactory() {
// Some standard functions.
   addFunc("PowerLaw", new PowerLaw(), false);
   addFunc("BrokenPowerLaw", new BrokenPowerLaw(), false);
   addFunc("Gaussian", new Gaussian(), false);
   addFunc("AbsEdge", new AbsEdge(), false);
   addFunc("ConstantValue", new ConstantValue(), false);
}

FunctionFactory::~FunctionFactory() {
   std::map<std::string, Function *>::iterator it = m_prototypes.begin();
   for (; it != m_prototypes.end(); it++)
      delete it->second;
}

void FunctionFactory::addFunc(const std::string &name, 
                              optimizers::Function* func, 
                              bool fromClone) {
   if (m_prototypes.count(name)) {
      std::cerr << "FunctionFactory::addFunc: A Function named "
                << name << " already exists.  Replacing it." 
                << std::endl;
   }
   if (fromClone) {
      m_prototypes[name] = func->clone();
   } else {
      m_prototypes[name] = func;
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

void FunctionFactory::getFunctionNames(std::vector<std::string> &funcNames) {
   funcNames.clear();
   std::map<std::string, Function *>::const_iterator it = m_prototypes.begin();
   for ( ; it != m_prototypes.end(); it++) {
      funcNames.push_back(it->first);
   }
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
//      funcObj->setName(name);
// Use the type attribute as the name for use by writeXml as the type
// information.
      funcObj->setName(type);

// Fetch the parameter elements and set the Parameter data members.
      std::vector<DOM_Element> params;
      Dom::getElements(*funcIt, "parameter", params);
      
      std::vector<DOM_Element>::const_iterator paramIt = params.begin();
      for (; paramIt != params.end(); paramIt++) {
         Parameter parameter;
         parameter.extractDomData(*paramIt);
         funcObj->setParam(parameter);
      }
      addFunc(name, funcObj, false);
   }
   delete parser;
}

void FunctionFactory::writeXml(const std::string &xmlFile) {
// This DOM_Document object doesn't get written out directly; it is
// used to create DOM_Elements, since the DOM_Element constructors do
// not allow the tag names to be specified.
   DOM_Document doc = DOM_Document::createDocument();

// The base DOM_Element.
   DOM_Element funcLib;
   funcLib = doc.createElement("function_library");
   funcLib.setAttribute("title", "prototype Functions");

// Loop over the Function prototypes, keeping only the derived prototypes.
   std::map<std::string, Function *>::iterator funcIt = m_prototypes.begin();
   for ( ; funcIt != m_prototypes.end(); funcIt++) {
      DOM_Element funcElt = doc.createElement("function");
      std::string name = funcIt->first;
      funcElt.setAttribute("name", name.c_str());
      std::string type = funcIt->second->getName();
      if (type == std::string("")) {
// Skip this Function since a lack of type implies a base prototype.
         continue;
      } else {
//         funcElt.setAttribute("type", type.c_str());
// Use the generic name of the Function object as the type attribute.
         funcElt.setAttribute("type", funcIt->second->genericName().c_str());
      }

      funcIt->second->appendParamDomElements(doc, funcElt);

      funcLib.appendChild(funcElt);
   }

// Write the XML file using the static function.
   std::ofstream outFile(xmlFile.c_str());
   outFile << "<?xml version='1.0' standalone='no'?>\n"
           << "<!DOCTYPE function_library SYSTEM \"FunctionModels.dtd\" >\n";
   xml::Dom::prettyPrintElement(funcLib, outFile, "");
}

} // namespace optimizers
