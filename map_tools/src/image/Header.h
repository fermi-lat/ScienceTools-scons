/** @file Header.h
    @brief definition of class Header

     @author Toby Burnett
     Code orginally written by Riener Rohlfs

     $Header$
*/
#ifndef HEADER_H
#define HEADER_H

#include "Attr.h"
#include <typeinfo>
#include <stdexcept>
#include <string>
#include <vector>

/** @class Header
    @brief Base class for all data objects, contains the header information
*/
class Header : public  std::vector<BaseAttr*>
{
public:
    typedef std::vector<BaseAttr*>::iterator iterator;
    typedef std::vector<BaseAttr*>::const_iterator const_iterator;

   Header() {}
   virtual ~Header();
   void addAttribute(const BaseAttr& attribute, bool replace=true);
   const BaseAttr& operator[](const std::string & name) const;

   template<typename T> void getValue(const std::string & name,
                                      T &value) const {
      Header::const_iterator it = find(name);
      BaseAttr * my_attr = 0;
      if (it != end()) {
         my_attr = *it;
      }
      if (my_attr) {
         if (dynamic_cast<Attr<T> *>(my_attr)) {
            value = dynamic_cast<Attr<T> *>(my_attr)->value();
         } else {
            throw std::runtime_error("Header::getValue: bad cast of type "
                                     + std::string(typeid(value).name()));
         }
      } else {
         throw std::invalid_argument("Header::getValue: attribute " 
                                     + name + " not found.");
      }         
   }

   void getAttributeNames(std::vector<std::string> & names) const;

   void setName(const std::string name){m_name=name;}

private:
    iterator find(const std::string& name);
    const_iterator find(const std::string& name)const;

   std::string m_name;
};

#endif
