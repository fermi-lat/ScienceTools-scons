/** @file Keyword.h
    @brief definition of class Keyword

// Modified version of Keydata.h written by Ben Dorman
// More descriptive credits are to follow

*/
#ifndef KEYWORDA_H
#define KEYWORDA_H


#include <string>

#include "Constants.h"
#include <iostream>

namespace table
{
    /** @class Keyword

    */
    class Keyword
    {
    public:
        class WrongKeywordValueType 
        {
        public:
            WrongKeywordValueType (const std::string& diag, bool silent=true)
            {
                if (!silent)
                    std::clog << "Wrong keyword type for key " << diag << std::endl;
            }
        };

        Keyword (const std::string &keyname,  ValueType keytype, const std::string &comment="")
            : m_name(keyname),
            m_type(keytype),
            m_comment(comment)
        {}


        Keyword(const Keyword &right)
            : m_name(right.m_name),
            m_type(right.m_type),
            m_comment(right.m_comment)
        {}

        Keyword & operator=(const Keyword &right);
        void copy(const Keyword& right);
        virtual Keyword *clone () const = 0;

        virtual ~Keyword()   {};


        virtual void get() =0;

        const std::string &name() const      { return m_name; }
        const ValueType &type() const        { return m_type; }
        const std::string &comment() const   { return m_comment; }

        void setName(const std::string &name)         { m_name = name; }
        void setType(const ValueType &type)           { m_type = type; }
        void setComment(const std::string &comment)   { m_comment = comment; }

        template <typename T>
            T &value(T &val) const;

        template <typename T>
            void setValue(const T &newValue);


    private: //## implementation
        std::string      m_name;
        ValueType        m_type;
        std::string      m_comment;

    };
#if 0  // Sandhia had this commented out

#incude "KeyData.h"

    template <typename T>
        inline void Keyword::setValue(const T &newValue)
    {
        try
        {
            KeyData<T>& thisKey = dynamic_cast<KeyData<T>&>(*this);
            thisKey.keyval(newValue);
        }
        catch (std::bad_cast)
        {
            throw Keyword::WrongKeywordValueType(name());
        }
    }


    template <typename T>
        inline T &Keyword::value (T &val) const
    {
        try
        {
            const KeyData<T> &thisKey = dynamic_cast<const KeyData<T>&>(*this);
            val = thisKey.keyval();
            return val;
        }
        catch (std::bad_cast)
        {
            throw Keyword::WrongKeywordValueType(name());
        }
    }
#endif
} // namespace table


#endif

