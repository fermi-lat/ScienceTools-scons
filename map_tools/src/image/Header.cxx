/**
 * @file Header.cxx
 * @brief Header class implementation.
 * @authors T. Burnett, J. Chiang
 * Original code from Riener Rohlfs
 *
 * $Header$
 */

#include "Header.h"

Header::~Header() {
   BaseAttrMap::iterator it = this->begin();
   for ( ; it!= this->end(); ++it) {
      delete it->second;
   }
}

void Header::addAttribute(const BaseAttr& attribute, bool replace) {
   std::string name = attribute.name();
   BaseAttrMap::iterator it = find(name);
   if (it == end()) {
      insert(BaseAttrMap::value_type(name, attribute.clone()));
      return;
   } 
   if (replace) {
      delete it->second;
      it->second = attribute.clone();
      return;
   } else {
      throw std::runtime_error("Header::addAttribute: attribute "
                               + name + " already exists.");
   }
}

const BaseAttr * Header::operator[](const std::string & name) const {
   BaseAttrMap::const_iterator it = find(name);
   if (it != end()) {
      return it->second;
   } else {
      return 0;
   }
}
