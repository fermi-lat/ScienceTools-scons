/** @file KeyUtil.cxx
*/
// KeyUtil.cxx

#include <sstream>
#include <algorithm>

#include "table/FitsService.h"
#include "table/Exception.h"
#include "table/KeyUtil.h"



using namespace table;

// static declaration
KeyUtil	*KeyUtil::s_instance = 0;
namespace {

    template<typename T>
    struct matchName : public std::unary_function<T, bool>
    {
        matchName(const std::string &name) : m_name(name) {}
        bool operator() (const std::pair<std::string, Keyword *> &x)
        {
            return (x.second->name() == m_name);
        }

        std::string  m_name;
    };

}


// instance()
// Returns the singleton instance of the class
KeyUtil	*KeyUtil::instance() 
{	 
    return (s_instance != 0) ? s_instance : (s_instance = new KeyUtil()); 
}


// kill()
// deletes and releases memory associated with the instance of this class
void KeyUtil::kill()
{
    delete s_instance;
    s_instance = 0;
}


Keyword *KeyUtil::readKey(const FitsService *ioService, int keynum)
{
    std::string key;
    std::string value;
    std::string comment;

    ioService->readKey(keynum, key, value, comment);

    if ((value != "") && (ioService->keyClassOK(key)))
    {
        char keyType = 'C';
        if (key != "HISTORY")
            keyType = ioService->keyType(value);

        return parseRecord(keynum, key, keyType, value, comment);
    }
    else
        return 0;
}


Keyword *KeyUtil::parseRecord(int keynum, const std::string &key, 
                              const char keyType, const std::string &valueString, 
                              const std::string &comment)
{
    std::string value("");
    bool bvalue(0);
    double fvalue(0);
    int ivalue(0);

    if (valueString[0] == '\'')
    {
        value = valueString.substr(1,valueString.length()-2);
    }

    else
    {
        value = valueString;       
    }

    std::istringstream vstream(value);

    switch(keyType)
    {
    case 'L':
        value == "T" ? bvalue = true : bvalue = false;
        return  new KeyData<bool>(key, Tlogical, bvalue, comment);
        break;
    case 'F':
        vstream >> fvalue;
        //return  new KeyData<float>(key, Tfloat, fvalue, comment);
        return  new KeyData<double>(key, Tdouble, fvalue, comment);
        break;
    case 'I':
    case 'T':
        vstream >> ivalue;
        return  new KeyData<int>(key, Tint, ivalue, comment);
        break;
    case 'C':
    default:
        {
            if (key != "HISTORY")
            {
                return  new KeyData<std::string>(key, Tstring, 
                    value.substr(0,value.find_last_not_of(" ")+1), comment);   
            }

            else
            {
                if (value.find("File modified by") == std::string::npos)
                {
                    std::ostringstream os;
                    os << "HISTORY" << keynum;
                    return  new KeyData<std::string>(os.str(), Tstring, 
                        value.substr(0,value.find_last_not_of(" ")+1), comment);   
                }

                else
                    return 0;
            }
        }
    }
}


void KeyUtil::writeKey(const FitsService *ioService, const Keyword *keyword)
{
    switch(keyword->type())
    {
    case Tstring:
        {
            std::string  val;
            if (keyword->name().find("HISTORY") == std::string::npos)
                ioService->writeKey(keyword->name(), keyword->value(val), keyword->comment());
            else
                ioService->writeHistory(keyword->value(val));

            break;
        }

    case Tdouble:
        {
            double  val;
            ioService->writeKey(keyword->name(), keyword->value(val), keyword->comment());
            break;
        }

    case Tfloat:
        {
            float  val;
            ioService->writeKey(keyword->name(), keyword->value(val), keyword->comment());
            break;
        }

    case Tlong:
        {
            long  val;
            ioService->writeKey(keyword->name(), keyword->value(val), keyword->comment());
            break;
        }

    case Tint:
        {
            int  val;
            ioService->writeKey(keyword->name(), keyword->value(val), keyword->comment());
            break;
        }

    case Tshort:
        {
            short  val;
            ioService->writeKey(keyword->name(), keyword->value(val), keyword->comment());
            break;
        }

    case Tlogical:
        {
            bool  val;

            std::string nm=keyword->name();
            if (nm=="PSR_COLS" || nm=="MC_TRUTH")
            {
                bool v=keyword->value(val);
                std::cout << "nm: " << nm << " v: " << v << std::endl;
            }

            ioService->writeKey(keyword->name(), keyword->value(val), 
                keyword->comment());
            break;
        }

    default:
        throw Exception("KeyUtil::writeKey:Invalid key format specified.\n");
    }
}


void KeyUtil::copyKey(const Keyword *keyword, std::map<std::string, Keyword *> &hdrKeyMap) throw()
{
    Keyword *pKeyword=keywd(keyword->name(), hdrKeyMap);
    if (pKeyword)
    {
        pKeyword->setName(keyword->name());
        pKeyword->setType(keyword->type());
        pKeyword->setComment("");   // set to null to preserve the value read from the template

        switch(keyword->type())
        {
        case Tlogical:
            {
                bool b=false;
                pKeyword->setValue(keyword->value(b));
                break;
            }

        case Tdouble:
            {
                double f=0;
                pKeyword->setValue(keyword->value(f));
                break;
            }

        case Tfloat:
            {
                float f=0;
                pKeyword->setValue(keyword->value(f));
                break;
            }

        case Tlong:
            {
                long i=0;
                pKeyword->setValue(keyword->value(i));
                break;
            }

        case Tint:
            {
                int i=0;
                pKeyword->setValue(keyword->value(i));
                break;
            }

        case Tstring:
            {
                std::string s="";
                pKeyword->setValue(keyword->value(s));
                break;
            }

        default:
            break;
        }
    }

    //else
    //std::cout << "Invalid key: " << keyword->name() << std::endl;
}


void KeyUtil::copyKey(const std::map<std::string, Keyword *> &inKeyMap, 
                      std::map<std::string, Keyword *> &outKeyMap) throw()
{
    std::map<std::string, Keyword *>::const_iterator it_end = inKeyMap.end();
    for (std::map<std::string, Keyword *>::const_iterator it=inKeyMap.begin(); it!=it_end; ++it)
    {
        Keyword *pKeyword=keywd(it->first, outKeyMap);
        if (pKeyword)
        {
            pKeyword->setName(it->second->name());
            pKeyword->setType(it->second->type());
            pKeyword->setComment("");   // set to null to preserve the value read from the template


            switch(pKeyword->type())
            {
            case Tlogical:
                {
                    bool b=false;
                    pKeyword->setValue(it->second->value(b));
                    break;
                }

            case Tfloat:
                {
                    float f=0;
                    pKeyword->setValue(it->second->value(f));
                    break;
                }

            case Tlong:
                {
                    long i=0;
                    pKeyword->setValue(it->second->value(i));
                    break;
                }

            case Tint:
                {
                    int i=0;
                    pKeyword->setValue(it->second->value(i));
                    break;
                }

            case Tstring:
                {
                    std::string s="";
                    pKeyword->setValue(it->second->value(s));
                    break;
                }

            default:
                break;
            }
        }

        //else
        //std::cout << "Invalid key: " << it->first << std::endl;
    }
}


Keyword *KeyUtil::keywd(const std::string &key, const std::map<std::string, Keyword *> &inKeyMap) throw()
{
    std::map<std::string, Keyword *>::const_iterator it = std::find_if(inKeyMap.begin(), inKeyMap.end(), 
        matchName<Keyword *>(key));

    if (it == inKeyMap.end())
        return 0;

    return it->second;
}


void KeyUtil::copyDSkeys(const std::map<std::string, Keyword *> &inKeyMap, 
                         std::map<std::string, Keyword *> &outKeyMap) throw()
{
    Keyword *inKeyword=keywd("NDSKEYS", inKeyMap);
    if (inKeyword)
    {
        if (!keywd(inKeyword->name(), outKeyMap))
            outKeyMap["NDSKEYS"] = new KeyData<int>(inKeyword->name(), Tint, 0);
        copyKey(inKeyword, outKeyMap);

        int ndsKeys=0;
        ndsKeys = inKeyword->value(ndsKeys);
        std::ostringstream os;

        for (int i=1; i<=ndsKeys; ++i)
        {
            os.str("");
            os << "DSTYP" << i;
            inKeyword = keywd(os.str(), inKeyMap);
            if (inKeyword)
            {
                if (!keywd(inKeyword->name(), outKeyMap))
                    outKeyMap[os.str()] = new KeyData<std::string>(inKeyword->name(), Tstring, "");
                copyKey(inKeyword, outKeyMap);
            }

            os.str("");
            os << "DSUNI" << i;
            inKeyword = keywd(os.str(), inKeyMap);
            if (inKeyword)
            {
                if (!keywd(inKeyword->name(), outKeyMap))
                    outKeyMap[os.str()] = new KeyData<std::string>(inKeyword->name(), Tstring, "");
                copyKey(inKeyword, outKeyMap);
            }

            os.str("");
            os << "DSVAL" << i;
            inKeyword = keywd(os.str(), inKeyMap);
            if (inKeyword)
            {
                if (!keywd(inKeyword->name(), outKeyMap))
                    outKeyMap[os.str()] = new KeyData<std::string>(inKeyword->name(), Tstring, "");
                copyKey(inKeyword, outKeyMap);
            }
        }
    }
}


void KeyUtil::copyHistoryKeys(const std::map<std::string, Keyword *> &inKeyMap, 
                              std::map<std::string, Keyword *> &outKeyMap) throw()
{
    std::map<std::string, Keyword *>::const_iterator it_begin=inKeyMap.begin();
    std::map<std::string, Keyword *>::const_iterator it_end  =inKeyMap.end();

    for (std::map<std::string, Keyword *>::const_iterator it=it_begin; it!=it_end; ++it)
    {
        if ((it->first).find("HISTORY") != std::string::npos)
        {
            if (!keywd(it->first, outKeyMap))
                outKeyMap[it->first] = new KeyData<std::string>(it->first, Tstring, "");
            copyKey(it->second, outKeyMap);
        }
    }
}


bool KeyUtil::key(const std::map<std::string, Keyword *> &inKeyMap, const std::string &keyname)
{
    std::map<std::string, Keyword *>::const_iterator it = std::find_if(inKeyMap.begin(), inKeyMap.end(), 
        matchName<Keyword *>(keyname));

    return (it == inKeyMap.end());
}


