// $Header$
/// Test program for serialization of DOM, stripping of comments

#include "xmlBase/Dom.h"
#include "xmlBase/XmlParser.h"

#include "facilities/commonUtilities.h"

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>

#include <string>
#include <iostream>

unsigned stripAndWrite(const std::string& fname, bool standalone=false);

int main() {
    
  // File is well-formed, no reference to dtd or schema
  std::string WFfile = facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "test.xml");
  stripAndWrite(WFfile);

  // Here's a file with embedded dtd
  std::string embeddedDtd = facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "test-dtd.xml");
  stripAndWrite(embeddedDtd); 

  // Now do it again for test file referencing dtd
  std::string refDtd = facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "myIFile.xml");
  stripAndWrite(refDtd, true);

  // One last time for test file referencing schema rather than dtd
  std::string refSchema = facilities::commonUtilities::joinPath(facilities::commonUtilities::getXmlPath("xmlBase"), "mySchemaIFile.xml");
  stripAndWrite(refSchema);
  
  return(0);
}

unsigned stripAndWrite(const std::string& fname, bool standalone) {
  XERCES_CPP_NAMESPACE_USE
  using xmlBase::Dom;

  xmlBase::XmlParser parser;

  std::string outname = fname + std::string("-stripped");

  DOMDocument* doc = parser.parse(fname.c_str());
  if (!doc) {
    std::cerr << "Parse of file " << fname << " failed" << std::endl;
    return 1;
  }

  DOMElement* docElt = doc->getDocumentElement();
  //  Dom::stripComments(docElt);
  Dom::stripComments(doc);

  bool status = Dom::writeIt(doc, outname.c_str(), standalone);
  return (status) ? 0 : 1;
}
  
  
