/** @file Primary.cxx


*/
#include "table/FitsService.h"
#include "table/KeyUtil.h"
#include "table/KeyData.h"
#include "table/Exception.h"

#include "table/PrimaryHDU.h"

namespace 
{
    const std::string Extname="Primary";

    const int                NLATKEYS  =5;
    const char               *LatKeys[]={"TELESCOP",     "INSTRUME",     "DATE-OBS",     "DATE-END",    
        "CREATOR"};

    const table::ValueType   LatType[] ={table::Tstring, table::Tstring, table::Tstring, table::Tstring, 
        table::Tstring}; 
}

using namespace table;

Primary::Primary()
: m_headerKeys(),
m_extname(Extname),
m_bufferSize(BUFFERSIZE),
m_extend(true),
m_naxis(0),
m_naxes(),
m_bitpix(0),
m_zero(0),
m_scale(0)
{
    loadHeaderKeys();
}


Primary::Primary( int bitpix, int naxis, const std::vector<long> &naxes,
                 bool extend, double zero, double scale)
                 : m_headerKeys(),
                 m_extname(Extname),
                 m_bufferSize(BUFFERSIZE),
                 m_extend(extend),
                 m_naxis(naxis),
                 m_bitpix(bitpix),
                 m_zero(zero),
                 m_scale(scale)
{
    m_naxes.resize(naxes.size());
    m_naxes = naxes;
    loadHeaderKeys();
}


void Primary::swap(Primary &other) throw()
{
    std::swap(m_headerKeys, other.m_headerKeys);
    std::swap(m_extname, other.m_extname);
    std::swap(m_bufferSize, other.m_bufferSize);
    std::swap(m_extend, other.m_extend);
    std::swap(m_naxis, other.m_naxis);
    std::swap(m_bitpix, other.m_bitpix);
    std::swap(m_zero, other.m_zero);
    std::swap(m_scale, other.m_scale);

    m_naxes.resize(other.naxes().size());
    std::swap(m_naxes, other.m_naxes);
}


Primary::Primary(const Primary &right)
{
    if (this != &right) copy(right);
}


Primary &Primary::operator=(const Primary &right)
{
    if (this != &right) copy(right);
    return *this;
}

void Primary::copy(const Primary &right)
{
    std::map<std::string, table::Keyword *>::const_iterator it_begin = right.m_headerKeys.begin();
    std::map<std::string, table::Keyword *>::const_iterator it_end   = right.m_headerKeys.end();

    for (std::map<std::string, Keyword *>::const_iterator it=it_begin; it!=it_end; ++it)
        m_headerKeys[it->first] = it->second->clone();

    m_bufferSize     = right.m_bufferSize;
}


Primary *Primary::clone() const   
{ 
    //return new Primary(*this); 
    if (m_bitpix == Idouble)
    {
        return dynamic_cast<PrimaryHDU<double> *>(const_cast<Primary *>(this))->clone();
    }

    else if (m_bitpix == Ifloat)
    {
        return dynamic_cast<PrimaryHDU<float> *>(const_cast<Primary *>(this))->clone();
    }

    else if (m_bitpix == Ilong)
    {
        return dynamic_cast<PrimaryHDU<long> *>(const_cast<Primary *>(this))->clone();
    }       

    else if (m_bitpix == Iulong)
    {
        return dynamic_cast<PrimaryHDU<unsigned long> *>(const_cast<Primary *>(this))->clone();
    }       

    else if (m_bitpix == Ishort)
    {
        return dynamic_cast<PrimaryHDU<short> *>(const_cast<Primary *>(this))->clone();
    }    

    else if (m_bitpix == Iushort)
    {
        return dynamic_cast<PrimaryHDU<unsigned short> *>(const_cast<Primary *>(this))->clone();
    }    

    else if (m_bitpix == Ibyte)
    {
        return dynamic_cast<PrimaryHDU<char> *>(const_cast<Primary *>(this))->clone();
    }    

    else
    {
        return new Primary(*this); 
    }        
}


Primary::~Primary()
{ 
    std::map<std::string, Keyword *>::const_iterator it_begin = m_headerKeys.begin();
    std::map<std::string, Keyword *>::const_iterator it_end   = m_headerKeys.end();

    for (std::map<std::string, Keyword *>::const_iterator it=it_begin; it!=it_end; ++it)
        delete it->second;
}


void Primary::setImageHeader(bool, bool extend, long naxis,
                             const std::vector<long> &naxes, int bitpix, double zero, double scale)
{
    setExtend(extend);
    setNaxis(naxis);
    setNaxes(naxes);
    setBitpix(bitpix);
    setZero(zero);
    setScale(scale);
}


void Primary::loadHeaderKeys()
{
    std::vector<std::string> keys;
    std::vector<ValueType> keyType;

    keys.resize(NLATKEYS);
    keyType.resize(NLATKEYS);
    std::copy(LatKeys, LatKeys+NLATKEYS, keys.begin());
    std::copy(LatType, LatType+NLATKEYS, keyType.begin());


    if (m_headerKeys.empty())
    {
        size_t keySize = keys.size();
        for (size_t i=0; i<keySize; ++i)
        {
            switch(keyType[i])
            {
            case Tstring:
                {
                    m_headerKeys[keys[i]] = new KeyData<std::string>(keys[i], keyType[i], "");
                    break;
                }

            case Tint:
            case Tlong:
                {
                    m_headerKeys[keys[i]] = new KeyData<long>(keys[i], keyType[i], 0);
                    break;
                }

            case Tfloat:
            case Tdouble:
                {
                    m_headerKeys[keys[i]] = new KeyData<double>(keys[i], keyType[i], 0.);
                    break;
                }
	    default: // see if this helps the gcc warning
	      ;
            }
        }
    }      
}

void Primary::readHeaderKeys(const FitsService* ioService)
{
    int extend;
    m_naxes.resize(10); 
    ioService->readImageHeader( m_bitpix, m_naxis,
        m_naxes, extend, m_zero, m_scale);
    m_extend=extend!=0;
}

void Primary::readAllKeys(const FitsService *ioService)
{
    ioService->makePrimaryCurrent();
    int numKeys = ioService->numKeys();

    KeyUtil *keyUtil = KeyUtil::instance();

    for (int i=1; i<=numKeys; ++i)
    {
        Keyword *keyword = keyUtil->readKey(ioService, i);
        if (keyword)
            m_headerKeys.insert(std::map<std::string,Keyword *>::value_type(keyword->name(),keyword->clone()));
    }
}


void Primary::read(const FitsService *ioService, long firstRow, long nrows, long firstElem)
{
    if (m_naxis > 0)
    {
        if (m_bitpix == Idouble)
        {
            dynamic_cast<PrimaryHDU<double> *>(this)->read(ioService, firstRow, nrows, firstElem);
        }    

        else if (m_bitpix == Ifloat)
        {
            dynamic_cast<PrimaryHDU<float> *>(this)->read(ioService, firstRow, nrows, firstElem);
        }    

        else if (m_bitpix == Ilong)
        {
            dynamic_cast<PrimaryHDU<long> *>(this)->read(ioService, firstRow, nrows, firstElem);
        }       

        else if (m_bitpix == Iulong)
        {
            dynamic_cast<PrimaryHDU<unsigned long> *>(this)->read(ioService, firstRow, nrows, firstElem);
        }       

        else if (m_bitpix == Ishort)
        {
            dynamic_cast<PrimaryHDU<short> *>(this)->read(ioService, firstRow, nrows, firstElem);
        }

        else if (m_bitpix == Iushort)
        {
            dynamic_cast<PrimaryHDU<unsigned short> *>(this)->read(ioService, firstRow, nrows, firstElem);
        }

        else if (m_bitpix == Ibyte)
        {
            dynamic_cast<PrimaryHDU<char> *>(this)->read(ioService, firstRow, nrows, firstElem);
        }

        else
            throw Exception("Primary::read:Invalid primary image format.\n");
    }
}


void Primary::writeAllKeys(const FitsService *ioService)
{
    KeyUtil *keyUtil = KeyUtil::instance();

    std::map<std::string, Keyword *>::const_iterator it_end = m_headerKeys.end();
    for(std::map<std::string, Keyword *>::const_iterator it=m_headerKeys.begin(); it!=it_end; ++it)
        keyUtil->writeKey(ioService, it->second);
}


void Primary::write(const FitsService *ioService, long firstRow, long)
{
    ioService->makePrimaryCurrent();
    writeAllKeys(ioService);

    if (m_naxis > 0)
    {
        if (m_bitpix == Idouble)
        {
            dynamic_cast<PrimaryHDU<double> *>(this)->write(ioService, firstRow);
        }    

        else if (m_bitpix == Ifloat)
        {
            dynamic_cast<PrimaryHDU<float> *>(this)->write(ioService, firstRow);
        }    

        else if (m_bitpix == Ilong)
        {
            dynamic_cast<PrimaryHDU<long> *>(this)->write(ioService, firstRow);
        }       

        else if (m_bitpix == Iulong)
        {
            dynamic_cast<PrimaryHDU<unsigned long> *>(this)->write(ioService, firstRow);
        }       

        else if (m_bitpix == Ishort)
        {
            dynamic_cast<PrimaryHDU<short> *>(this)->write(ioService, firstRow);
        }

        else if (m_bitpix == Iushort)
        {
            dynamic_cast<PrimaryHDU<unsigned short> *>(this)->write(ioService, firstRow);
        }

        else if (m_bitpix == Ibyte)
        {
            dynamic_cast<PrimaryHDU<char> *>(this)->write(ioService, firstRow);
        }

        else
            throw Exception("Primary::write:Invalid primary image format.\n");
    }
}

