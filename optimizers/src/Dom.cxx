#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>

#include "optimizers/Dom.h"

namespace {
   using XERCES_CPP_NAMESPACE_QUALIFIER XMLString;

   class XStr {
   public:
      XStr(const char * const toTranscode) {
         m_unicodeForm = XMLString::transcode(toTranscode);
      }
      XStr(const std::string & toTranscode) {
         m_unicodeForm = XMLString::transcode(toTranscode.c_str());
      }
      ~XStr() {
         XMLString::release(&m_unicodeForm);
      }
      const XMLCh * unicodeForm() const {
         return m_unicodeForm;
      }
   private:
      XMLCh * m_unicodeForm;
   };
}

namespace optimizers {
   XERCES_CPP_NAMESPACE_USE

   DOMDocument * Dom::createDocument() {
      DOMImplementation * impl = DOMImplementationRegistry::
         getDOMImplementation(::XStr("Core").unicodeForm());
      DOMDocument * doc = impl->createDocument();
      return doc;
   }

   DOMElement * Dom::createElement(DOMDocument * doc, 
                                   const std::string & name) {
      DOMElement * elt = doc->createElement(::XStr(name).unicodeForm());
      return elt;
   }

   void Dom::appendChild(DOMNode * parent, DOMElement * child) {
      parent->appendChild(reinterpret_cast<DOMNode *>(child));
   }

   void Dom::appendChild(DOMElement * parent, DOMElement * child) {
      parent->appendChild(reinterpret_cast<DOMNode *>(child));
   }
}
