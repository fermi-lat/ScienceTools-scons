// Abstract class

#ifndef PRIMARY_H
#define PRIMARY_H

#include "table/Keyword.h"
#include "table/KeyData.h"

#include <vector>
#include <string>
#include <map>

namespace table {
class FitsService;
class Keyword;
}

namespace table{
class Primary
{
public:    
    Primary();
    Primary( int bitpix, int naxis, const std::vector<long> &naxes,
        bool extend, double zero, double scale);
    virtual ~Primary();

    Primary(const Primary &right);
    Primary &operator=(const Primary &right);
    virtual Primary *clone() const;


    // Accessors
    const std::map<std::string, Keyword *> &headerKeys() const;
    std::map<std::string, Keyword *> &headerKeys();


    virtual const std::string &extname() const;
    virtual const long bufferSize() const;
    virtual const bool extend() const;
    virtual const long naxis() const;
    virtual const std::vector<long> &naxes() const;
    virtual const int bitpix() const;
    virtual const double zero() const;
    virtual const double scale() const;


    virtual void setBufferSize(long bufferSize);
    virtual void setExtend(bool extend);
    virtual void setNaxis(long naxis);
    virtual void setNaxes(const  std::vector<long> &naxes);
    virtual void setBitpix(int bitpix);
    virtual void setZero(double zero);
    virtual void setScale(double scale);
    virtual void setImageHeader(bool simple, bool extend, long naxis,
        const std::vector<long> &naxes, int bitpix, double zero, double scale);


    // Data IO Methods
    virtual void read(const FitsService *ioService, long firstRow=1, long nrows=-1,
        long firstElem=1);
    
    virtual void readHeaderKeys(const FitsService* ioService);


    virtual void write(const FitsService *ioService, long firstRow=1, long firstElem=1);
    void readAllKeys(const FitsService *ioService);

    void setKey(const std::string& name, std::string value){
            m_headerKeys[name]=new KeyData<std::string>(name, Tstring, value);
        }
    void setKey(const std::string& name, double value){
            m_headerKeys[name]=new KeyData<double>(name, Tdouble, value);
        }

protected:    
    // Data Members
    std::map<std::string, Keyword *>  m_headerKeys;
    std::string                       m_extname;
    long                              m_bufferSize;
    bool                              m_extend;
    int                               m_naxis;
    std::vector<long>                 m_naxes;
    int                               m_bitpix;
    double                            m_zero;
    double                            m_scale;


    enum {BUFFERSIZE=100000};


    virtual void loadHeaderKeys();

    void copy(const Primary &right);
    void swap(Primary &other) throw();

    virtual void writeAllKeys(const FitsService *ioService);
};


inline const std::string &Primary::extname() const
{	
    return m_extname; 
}


inline const std::map<std::string, Keyword *> &Primary::headerKeys() const
{	
    return m_headerKeys; 
}


inline std::map<std::string, Keyword *> &Primary::headerKeys()
{	
    return m_headerKeys; 
}


inline const long Primary::bufferSize() const
{
    return m_bufferSize;
}




inline const bool Primary::extend() const
{
    return m_extend;
}


inline const long Primary::naxis() const
{
    return m_naxis;
}


inline const std::vector<long> &Primary::naxes() const
{
    return m_naxes;
}


inline const int Primary::bitpix() const
{
    return m_bitpix;
}


inline const double Primary::zero() const
{
    return m_zero;
}


inline const double Primary::scale() const
{
    return m_scale;
}


inline void Primary::setBufferSize(long bufferSize)
{
    m_bufferSize = bufferSize;
}




inline void Primary::setExtend(bool extend)
{
    m_extend = extend;
}


inline void Primary::setNaxis(long naxis)
{
    m_naxis = naxis;
}


inline void Primary::setNaxes(const std::vector<long> &naxes)
{
    m_naxes.resize(naxes.size());
    m_naxes = naxes;
}


inline void Primary::setBitpix(int bitpix)
{
    m_bitpix = bitpix;
}


inline void Primary::setZero(double zero)
{
    m_zero = zero;
}


inline void Primary::setScale(double scale)
{
    m_scale = scale;
}

} // table
#endif //PRIMARY_H
