/**
 * @file Header.cxx
 * @brief Header class implementation.
 * @authors T. Burnett, J. Chiang
 * Original code from Riener Rohlfs
 *
 * $Header$
 */

#include "Header.h"
#include <algorithm>

Header::~Header() {
   Header::iterator it = this->begin();
   for ( ; it!= this->end(); ++it) {
       delete *it;
   }
}
Header::iterator Header::find(const std::string& name)
{
   Header::iterator it = begin();
    for(; it!=end(); ++it){
        BaseAttr& attr = **it;
        if( attr.name() == name) break;
    }
    return it;
}
Header::const_iterator Header::find(const std::string& name)const
{
   Header::const_iterator it = begin();
    for(; it!=end(); ++it){
        BaseAttr& attr = **it;
        if( attr.name() == name) break;
    }
    return it;
}
void Header::addAttribute(const BaseAttr& attribute, bool replace) {

   Header::iterator it = find(attribute.name());
   if( it==end() || ! replace){
       this->push_back(attribute.clone());
       return;
   }
   if( replace) {
       delete *it;
       *it = attribute.clone();
       return;
   }
   throw std::runtime_error("Header::addAttribute: attribute "
                               + attribute.name() + " already exists.");

}


const BaseAttr & Header::operator[](const std::string & name) const {
   Header::const_iterator it = find( name);
   if( it!= end() ) return **it;

   throw std::runtime_error("Header::::operator[]: attribute "
                               + name + " not found.");
}

void Header::getAttributeNames(std::vector<std::string> & names) const {
   names.clear();
   Header::const_iterator it = begin();
   for ( ; it != end(); ++it) {
      names.push_back((*it)->name());
   }
}
