#ifndef optimizers_Dom_h
#define optimizers_Dom_h

#include <string>

#include <xercesc/util/XercesDefs.hpp>

XERCES_CPP_NAMESPACE_BEGIN
class DOMElement;
class DOMDocument;
class DOMNode;
XERCES_CPP_NAMESPACE_END

namespace optimizers {

using XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument;
using XERCES_CPP_NAMESPACE_QUALIFIER DOMElement;
using XERCES_CPP_NAMESPACE_QUALIFIER DOMNode;

class Dom {

public:

   static DOMDocument * createDocument();

   static DOMElement * createElement(DOMDocument * doc, 
                                     const std::string & name);

   static void appendChild(DOMNode * parent, DOMElement * child);

   static void appendChild(DOMElement * parent, DOMElement * child);

};

} // namespace optimizers

#endif // optimizers_Dom_h
