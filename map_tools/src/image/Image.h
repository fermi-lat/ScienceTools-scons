/** @file Image.h
    @brief definition of classes Image<class T> and Header

     @author Toby Burnett
     Code orginally written by Riener Rohlfs

     $Header$
*/

#ifndef ROOT_TFImage_H
#define ROOT_TFImage_H

#include "IOElement.h"

template <class T> class Image;
#include <vector>

//_____________________________________________________________________________
/** @class BaseImage
    @brief abstract base class for templated Image  
*/
class BaseImage : public IOElement
{

protected:
    /// protected default cto
    BaseImage(){};

    /// protected ctor without a file
    BaseImage(const std::string & name, const std::vector<long>& axes)
        :IOElement(name), m_axisSize(axes){};

    /// protected ctor that associates with a file
    BaseImage(const std::string & name, const std::string & fileName, const std::vector<long>& axes)
        :IOElement(name, fileName), m_axisSize(axes){};
public:
    virtual ~BaseImage(){};

    virtual bool isImage()const{return true;}

    const std::vector<long> & getAxisSize()const{return m_axisSize;}
    unsigned int   pixelCount()const;

    operator  Image<float> * ();

protected:
    static BaseImage* readImage(const std::string & fileName, const std::string & name,  
                              unsigned int cycle = 0, VirtualIO::FMode mode = VirtualIO::Read);
private:
    std::vector<long> m_axisSize; 
};

//_____________________________________________________________________________
/** @class Image
    @brief represents a hypercube of image planes  

    */
template <class T > 
class Image : public BaseImage
{

public:
    Image(){ clearNull();}

    /** @brief create a memory-only image 
        @param name name
        @param axes description of axis dimensions
        */
    Image(const std::string & name, const std::vector<long>& axes) 
        : BaseImage(name, axes)                   
    { m_data.resize(pixelCount()); clearNull();}

    /** @brief create an image associated with a file
        @param name
        @param fileName
        @param axes
        */
    Image(const std::string & name, const std::string & fileName, const std::vector<long>& axes) 
        : BaseImage(name, fileName, axes)                   
    {m_data.resize(pixelCount()); clearNull();  fio()->createElement();}

    ~Image(){}
    //! const access to data array
    const std::vector<T> &    data() const   {return m_data;}

    //! writable access to data
    std::vector<T> &    data()   {return m_data;}

    virtual T      getNull()         {return fNull;}
    virtual void   setNull(T null)   {fNull = null; fNullDefined = true;}
    virtual void   clearNull()       {fNullDefined = false;}
    virtual bool   nullDefined()     {return fNullDefined;}


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
