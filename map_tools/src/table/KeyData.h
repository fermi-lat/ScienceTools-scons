/** @file KeyData.h
    @brief definition of class KeyData

// Modified version of Keydata.h written by Ben Dorman
// More descriptive credits are to follow

*/
#ifndef KEYDATA_H
#define KEYDATA_H 1

#include <string>

#include "Keyword.h"
//#include <typeinfo.h>


namespace table
{
    template <typename T>
    class KeyData : public Keyword  
    {
    public:
        KeyData (const std::string &keyname,  ValueType keytype, const T &value, 
            const std::string &comment = "")
            : Keyword(keyname, keytype, comment), 
            m_keyval(value)
        {  
#if 0            // example of how to simplify the interface -- not finished
            if(typeid(value)==typeid(std::string)) keytype=Tstring;
            else if(typeid(value)==typeid(double)) keytype = Tdouble;
#endif
        }


        KeyData(const KeyData< T > &right);
        virtual KeyData <T> *clone() const;

        virtual ~KeyData()   {}

        //virtual void write();
        virtual void get()   {}

        const T &keyval() const       { return m_keyval; } 
        void keyval(const T &value)   { m_keyval = value; }


    private:
        T m_keyval;
    };

    template <typename T>
        KeyData <T>* KeyData<T>::clone() const
    {
        return new KeyData<T>(*this);
    }


    template <typename T>
        KeyData<T>::KeyData(const KeyData<T> &right)
        : Keyword(right),
        m_keyval(right.m_keyval)
    {}
} // namespace table



#endif

