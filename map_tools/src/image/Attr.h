/** @file Attr.h
    @brief definiton of BaseAttr and Attr<class T> classes

     @author Toby Burnett
     Code orginally written by Riener Rohlfs

    $Header$
*/
#ifndef ATTR_H
#define ATTR_H

#include <string>
#include <ostream>
//_____________________________________________________________________________
/** @class BaseAttr
    @brief abstract base class for attributes
    */
class BaseAttr 
{

public:
    BaseAttr() {};
    BaseAttr(const std::string& name, 
        const std::string& unit = "", 
        const std::string& comment = "" )
        : m_name(name), m_unit(unit), m_comment(comment)
    {}
    virtual BaseAttr * clone() const=0;

    virtual ~BaseAttr() {}
    const std::string name()const{return m_name;}
    const std::string comment()const{return m_comment;}
    const std::string unit()const{return m_unit;}

    void setName(const std::string& name){m_name=name;}
    virtual void print(std::ostream&)const=0;

    operator const std::string&()const{return m_name;}
    //virtual operator double()const = 0;

private:
    std::string m_name;
    std::string m_unit;
    std::string m_comment; 
};

//_____________________________________________________________________________

/** @class Attr
    @brief templated attribute class
*/

template <typename T > 
class Attr : public BaseAttr
{
public:
    Attr() {}

    Attr(const std::string & name,  T value,
        const std::string & unit    = "", 
        const std::string & comment = "" )
        : BaseAttr(name, unit, comment),m_value(value)
    {}
    virtual BaseAttr * clone() const{ return new Attr<T>(*this);}

    const T& value()const{return m_value;}
    virtual void print(std::ostream&out)const{ out << m_value;} 

private:
    T   m_value;   // The value of the attribute
};

typedef Attr<std::string> StringAttr;
typedef Attr<int> IntAttr;
typedef Attr<unsigned int> UintAttr;
typedef Attr<double> DoubleAttr;
typedef Attr<float> FloatAttr;
typedef Attr<bool> BoolAttr;

inline std::ostream& operator<<(std::ostream& out, const BaseAttr & attr){ attr.print(out); return out;}
#endif
