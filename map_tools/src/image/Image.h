/** @file Image.h

*/
#ifndef ROOT_TFImage_H
#define ROOT_TFImage_H

#include "IOElement.h"

template <class T> class Image;

//_____________________________________________________________________________

class BaseImage : public IOElement
{

public:
    BaseImage(){};
    BaseImage(const std::string & name, const std::vector<long>& axes)
        :IOElement(name), m_axisSize(axes){};

    BaseImage(const std::string & name, const std::string & fileName, const std::vector<long>& axes)
        :IOElement(name, fileName), m_axisSize(axes){};

    virtual ~BaseImage(){};

    virtual bool isImage()const{return true;}

    const std::vector<long> & getAxisSize()const{return m_axisSize;}
    unsigned int   pixelCount()const;

    operator  Image<float> * ();

protected:
    static BaseImage* readImage(const std::string & fileName, const std::string & name,  
                              unsigned int cycle = 0, VirtualIO::FMode mode = VirtualIO::kFRead);
private:
    std::vector<long> m_axisSize; 
};

//_____________________________________________________________________________

template <class T > 
class Image : public BaseImage
{

public:
    Image(){ clearNull();}

    Image(const std::string & name, const std::vector<long>& axes) 
        : BaseImage(name, axes)                   
    { m_data.resize(pixelCount()); clearNull();}

    Image(const std::string & name, const std::string & fileName, const std::vector<long>& axes) 
        : BaseImage(name, fileName, axes)                   
    {m_data.resize(pixelCount()); clearNull();  fio()->createElement();}


    ~Image(){}
    virtual T      getNull()         {return fNull;}
    virtual void   setNull(T null)   {fNull = null; fNullDefined = true;}
    virtual void   clearNull()       {fNullDefined = false;}
    virtual bool   nullDefined()     {return fNullDefined;}

    const std::vector<T> &    data() const   {return m_data;}
    std::vector<T> &    data()   {return m_data;}


private:
    std::vector<T> m_data;
    T        fNull;         // NULL value of this image
    bool   fNullDefined;  // true if a NULL value is defined

};


//_____________________________________________________________________________
//_____________________________________________________________________________

typedef    Image<float>    FloatImg;
typedef    Image<double>   DoubleImg;
typedef    Image<int>      IntImg;

//_____________________________________________________________________________
//_____________________________________________________________________________

inline BaseImage::operator Image<float> * () {return dynamic_cast <FloatImg *>(this);}



#endif // ROOT_TFImage
