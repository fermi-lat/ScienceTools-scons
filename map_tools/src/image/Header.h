/** @file Header.h
    @brief definition of class Header

     @author Toby Burnett
     Code orginally written by Riener Rohlfs

     $Header$
*/
#ifndef HEADER_H
#define HEADER_H

#include "Attr.h"
#include <vector>
#include <string>

/** @class Header
    @brief Base class for all data objects, contains the header information
*/
class Header : public std::vector< BaseAttr*>
{
public:
    Header()  {}
    virtual ~Header(){}; //TODO: delete objects in vector
    void addAttribute(const BaseAttr& attribute, bool /*replace*/=true){ //TODO: implement replace?
        push_back(attribute.clone());
    }
    void setName(const std::string name){m_name=name;}
private:
    std::string m_name;
};

#endif
