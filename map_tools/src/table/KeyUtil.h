/** @file KeyUtil.h

*/

#ifndef KEYUTIL_H
#define KEYUTIL_H

#include <algorithm>
#include <sstream>
#include <string>
#include <map>
#include <typeinfo>

//#include "table/Utilities.h"
#include "table/Exception.h"

#include "KeywordT.h"



namespace table
{
    class FitsService;
    class Keyword;

    class KeyUtil
    {
    public:
        static KeyUtil	*instance();
        static void kill ();

        static bool key(const std::map<std::string, Keyword *> &inKeyMap, const std::string &keyname);

        Keyword *readKey(const FitsService *ioService, int keynum);
        void writeKey(const FitsService *ioService, const Keyword *keyword);
        void copyKey(const Keyword *keyword, std::map<std::string, Keyword *> &hdrKeyMap) throw();
        void copyKey(const std::map<std::string, Keyword *> &inKeyMap, std::map<std::string, 
            Keyword *> &outKeyMap) throw();
        void copyDSkeys(const std::map<std::string, Keyword *> &inKeyMap, 
            std::map<std::string, Keyword *> &outKeyMap) throw();
        void copyHistoryKeys(const std::map<std::string, Keyword *> &inKeyMap, 
            std::map<std::string, Keyword *> &outKeyMap) throw();

        template<typename T>
            void getKey(const std::map<std::string, Keyword *> &hdrKeyMap, const std::string &key, T &value);
        template<typename T>
            void setKey(const std::map<std::string, Keyword *> &hdrKeyMap, const std::string &key, const T &value);
        template <typename T>
            void insertKey(std::map<std::string, Keyword *> &hdrKeyMap, const std::string &key, const T &value, 
            const std::string &comment=std::string(""));


    protected:
        KeyUtil()   {}
        virtual ~KeyUtil()   { ; }

    private:
        static KeyUtil* s_instance;


        Keyword *keywd(const std::string &key, const std::map<std::string, Keyword *> &inKeyMap) throw();
        Keyword *parseRecord (int keynum, const std::string &key, const char keyType, 
            const std::string &valueString, const std::string &comment);
    };


    template <typename T>
        void KeyUtil::getKey(const std::map<std::string, Keyword *> &hdrKeyMap, 
        const std::string &key, T &value)
    {
        std::map<std::string, Keyword *>::const_iterator it = std::find_if(hdrKeyMap.begin(), hdrKeyMap.end(), 
            Utilities::matchName<Keyword *>(key));
        if (it != hdrKeyMap.end())
            value = it->second->value(value); 

        else
        {
            throw Goodi::InvalidKey("");
            //DisplayMessage("KeyUtil::getKey:\nCannot read key "+key+".\n");
        }
    }


    template <typename T>
        void KeyUtil::setKey(const std::map<std::string, Keyword *> &hdrKeyMap, 
        const std::string &key, const T &value)
    {
        std::map<std::string, Keyword *>::const_iterator it = 
            std::find_if(hdrKeyMap.begin(), hdrKeyMap.end(), 
            Utilities::matchName<Keyword *>(key));
        if (it != hdrKeyMap.end())
            it->second->setValue(value);

        else
        {
            throw table::InvalidKey("");
            //DisplayMessage("KeyUtil::setKey:\nCannot set key "+key+".\n");
        }
    }


    template <typename T>
        void KeyUtil::insertKey(std::map<std::string, Keyword *> &hdrKeyMap, 
        const std::string &key, const T &value, const std::string &comment)
    {
        ValueType datatype;

        if (typeid(T) == typeid(std::string)) 
            datatype=Tstring;
        else if (typeid(T) == typeid(double))
            datatype=Tdouble;
        else if (typeid(T) == typeid(float))
            datatype=Tfloat;
        else if (typeid(T) == typeid(long))
            datatype=Tlong;
        else if (typeid(T) == typeid(int))
            datatype=Tint;
        else if (typeid(T) == typeid(short))
            datatype=Tshort;
        else if (typeid(T) == typeid(bool))
            datatype=Tlogical;
        else
        {
            std::ostringstream errorMessage;
            errorMessage << "KeyUtil::insertKey:\nInvalid key data type for " <<
                key << ".\n";
            throw Exception(errorMessage.str());
        }

        hdrKeyMap[key] = new KeyData<T>(key, datatype, value, const_cast<char *>(comment.c_str()));
    }
}   // namespace table

#endif //KEYUTIL_H
