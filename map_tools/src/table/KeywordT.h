/** @file KeywordT.h
    @brief definition of keyword value and setValue template functions

// Modified version of Keydata.h written by Ben Dorman
// More descriptive credits are to follow

*/
#ifndef KEYWORDT_H
#define KEYWORDT_H
#include <typeinfo>

#include "KeyData.h"


namespace table 
{
    template <typename T>
        T &Keyword::value (T &val) const
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

    template <typename T>
        void Keyword::setValue (const T &newValue)
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
} // namespace Goodi

#endif

