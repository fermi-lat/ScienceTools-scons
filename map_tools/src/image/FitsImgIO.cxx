/** @file FitsImgIO.cxx
    @brief definition of static functions for reading/writing FITS images

     @author Toby Burnett
     Code orginally written by Riener Rohlfs

     $Header$
    (TODO: make them static in the Fits_IO class)
*/
#include "Fits_IO.h"
#include "IOElement.h"
#include "Image.h"

#ifdef WIN32
# include <cmath>
# include <stdlib.h>
  static inline __int64 atoll(const char* c){return _atoi64(c);}
# include <limits.h>
# include <float.h>
#else
#include <values.h>
#endif
#include <stdexcept>
#include "fitsio.h"

static IOElement * ReadIntImage(fitsfile *fptr, std::vector<long> axes, int * status);
static IOElement * ReadFloatImage(fitsfile *fptr, std::vector<long> axes, int * status);
static IOElement * ReadDoubleImage(fitsfile *fptr, std::vector<long> axes, int * status);

//_____________________________________________________________________________
int CreateFitsImage(fitsfile* fptr, IOElement* element)
{
    // Creatres a new image in a FITS file

    int type= -10000;
    int bzero = 0;
    unsigned int ubzero = 0;

    try{
        dynamic_cast<FloatImg*>(element);     type = FLOAT_IMG;
    }catch(...){
        try{ 
            dynamic_cast<DoubleImg*>(element); type = DOUBLE_IMG;
        }catch(...){
            try{
                dynamic_cast<IntImg*>(element); type = LONG_IMG;
            }catch(...){
                throw std::invalid_argument("Unsupported image type");
            }
        }
  
    }
    BaseImage * image = dynamic_cast<BaseImage*>(element);
    long *  axis = const_cast<long*>(&*(image->getAxisSize().begin())); //ugly

    int status = 0;
    fits_create_img(fptr, type, image->getAxisSize().size(), axis, &status);

    if (bzero != 0)
    {
        fits_write_key(fptr, TINT, (char*)"BZERO", &bzero, 
            (char*)"Make values Signed", &status);
        bzero = 1;
        fits_write_key(fptr, TINT, (char*)"BSCALE", &bzero, 
            (char*)"Make values Signed", &status);
    }
    if (ubzero != 0)
    {
        fits_write_key(fptr, TUINT, (char*)"BZERO", &ubzero, 
            (char*)"Make values Unsigned", &status);
        ubzero = 1;
        fits_write_key(fptr, TUINT, (char*)"BSCALE", &ubzero, 
            (char*)"Make values Unsigned", &status);
    }


    fits_write_key(fptr, TSTRING, (char*)"EXTNAME", (void*)image->getName().c_str(), 
        (char*)"Extension name", &status);

    if (status != 0)
        Fits_IO::report_error(status);
    return status;
}


//_____________________________________________________________________________
IOElement * MakeImage(fitsfile * fptr, int * status)
{
    // Creates an Image and fills it with data from a FITS image.
    // fptr may point to the primary header without image. In this case
    // the function creates a IOElement. 
    // In any case the name of the returned element are set to "no name" and
    // should be overwritten if possible

    if (*status != 0) return NULL;

    int numDim;
    int dataType;
    long axisSize[9];   // the maximum number of dimension in a FITS file is 9

    // try to get number of dimension and the size of the image in each 
    // dimension
    *status = fits_get_img_param(fptr, 9, &dataType, &numDim, axisSize, status);

    if (*status != 0 || numDim == 0) {
        // this is proberly a primary header without data
        *status = 0;
        throw std::invalid_argument("file has no image");
    }
    std::vector<long> axes(numDim);
    std::copy(axisSize, axisSize+numDim, axes.begin());

    IOElement * image = 0;
    switch (dataType)
    {

   case LONG_IMG:
//       image = ReadIntImage(fptr,  axes, status);
       image = ReadFloatImage(fptr,  axes, status);
       break;
   case FLOAT_IMG:
       image = ReadFloatImage(fptr,  axes, status);
       break;

   case DOUBLE_IMG:
       image = ReadDoubleImage(fptr, axes, status);
       break;

   default:
       // should never happen
       throw std::runtime_error("Unsupported FITS image type "); 
           
       *status = -1;   
    }

    return image;
}


//_____________________________________________________________________________
static IOElement * ReadIntImage(fitsfile *fptr, std::vector<long> axes, int * status)
{
    if (*status != 0) return NULL;
#if 1 // still need to convert
    throw std::invalid_argument("Reading int image not fully implemented");
    return 0;
#else
    long tfSize[9];
    long   firstPixel[9];
    long   numPixel = 1;
    for (int dim = 0; dim < numDim; dim++)
    {
        numPixel *= fitsSize[dim];
        firstPixel[dim] = 1;
        tfSize[dim] = fitsSize[numDim - dim - 1];
    }


    char offset[30];
    fits_read_keyword(fptr, (char*)"BZERO", offset, NULL, status);

    if (*status == 0 && strcmp(offset, "2147483648") == 0)
    {
        // this is a unsigned integer column

        // is there a NULL value defined?
        char           strNullVal[20];
        unsigned int   nullVal = 0;
        int            anyNull;

        fits_read_keyword(fptr, (char*)"BLANK", strNullVal, NULL, status);
        if (*status == 0)
            nullVal = (unsigned int)( atoll(strNullVal) + 2147483648LL);
        else
            *status = 0;

        IntImg * image = new IntImg("no name", numDim, tfSize);

        fits_read_pix(fptr, TINT, firstPixel, numPixel, &nullVal, 
            image->GetDataArray(), &anyNull, status);

        if (*status != 0) 
        {
            //TFError::SetError("ReadIntImage", errMsg, *status, fptr->Fptr->filename); 
            delete image;
            image = NULL;
        }
        else if (anyNull)
            image->SetNull(nullVal);

        return image;
    }
    else
    {
        // this is a sigend short column
        *status = 0;

        // is there a NULL value defined?
        char        strNullVal[20];
        int         nullVal = 0;
        int         anyNull;

        fits_read_keyword(fptr, (char*)"BLANK", strNullVal, NULL, status);
        if (*status == 0)
            nullVal = atoi(strNullVal);
        else
            *status = 0;

        // we cannot read directly signed char. Therefore we read short
        TFIntImg * image = new TFIntImg("no name", numDim, tfSize);

        fits_read_pix(fptr, TINT, firstPixel, numPixel, &nullVal, 
            image->GetDataArray(), &anyNull, status);

        if (*status != 0) 
        {
            //TFError::SetError("ReadIntImage", errMsg, *status, fptr->Fptr->filename); 
            delete image;
            image = NULL;
        }
        else if (anyNull)
            image->SetNull(nullVal);

        return image;
    }
#endif

}
//_____________________________________________________________________________
static IOElement * ReadFloatImage(fitsfile *fptr,  std::vector<long> axes, int * status)
{
    if (*status != 0) return NULL;


    float nullVal = FLT_MAX;
    int anyNull;

    FloatImg * image = new Image<float>("no name", axes);

    float* data = &*image->data().begin();

    fits_read_img(fptr, TFLOAT, 1, image->pixelCount(), &nullVal, data, &anyNull, status);

    if (*status != 0) 
    {
        //TFError::SetError("ReadFloatImage", errMsg, *status, fptr->Fptr->filename); 
        delete image;
        image = NULL;
    }
    else if (anyNull)
        image->setNull(nullVal);

    return image;
}

//_____________________________________________________________________________
static IOElement * ReadDoubleImage(fitsfile *fptr,  std::vector<long> axes, int * status)
{
    if (*status != 0) return NULL;

    double nullVal = DBL_MAX;
    int anyNull;

    Image<double> * image = new Image<double>("no name", axes);
    double* data = &*image->data().begin();

    fits_read_img(fptr, TDOUBLE, 1, image->pixelCount(), &nullVal, 
        data, &anyNull, status);

    if (*status != 0) 
    {
        //TFError::SetError("ReadDoubleImage", errMsg, *status, fptr->Fptr->filename); 
        delete image;
        image = NULL;
    }
    else if (anyNull)
        image->setNull(nullVal);

    return image;
}

//_____________________________________________________________________________
template <class N, class I>
int WriteImage(fitsfile * fptr, I * image, int dataType, long * firstPixel,
               long numPixel, int status)
{
    // writes the values into the FITS image and sets NULL values correctly if
    // NULL values are defined in the image

    if (image->NullDefined())
    {
        N nullValue = image->GetNull();
        fits_update_key(fptr, dataType, (char*)"BLANK", &nullValue, 
            (char*)"NULL value", &status);
        fits_write_pixnull(fptr, dataType, firstPixel, numPixel, 
            image->GetDataArray(), &nullValue, &status);
    }
    else
        fits_write_pix(fptr, dataType, firstPixel, numPixel, 
        image->GetDataArray(), &status);

    return status;
}
//_____________________________________________________________________________
int SaveImage(fitsfile* fptr, BaseImage* image)
{
    // Save an image in a FITS file

    int status = 0;
    unsigned int numPixel = image->pixelCount();
#ifdef WIN32 // this does not work on Linux, assume only float for now
    const char* rawname = typeid(*image).raw_name();

    if( strcmp(rawname, typeid(FloatImg).raw_name())==0 ) {
#endif
        FloatImg * fimg = dynamic_cast<FloatImg*>(image);
        float nullval = fimg->getNull();
        float* data = &*fimg->data().begin();
        fits_write_imgnull(fptr, TFLOAT, 1, numPixel, 
            data, 
            fimg->nullDefined() ? &nullval : 0,
            &status);
#ifdef WIN32
    } else if( strcmp(rawname, typeid(DoubleImg).raw_name())==0 ) {

        DoubleImg * fimg = dynamic_cast<DoubleImg*>(image);
        double nullval = fimg->getNull();
        double* data = &*fimg->data().begin();
        fits_write_imgnull(fptr, TDOUBLE, 1, numPixel, 
            data, 
            fimg->nullDefined() ? &nullval : 0,
            &status);

    } else if( strcmp(rawname, typeid(IntImg).raw_name())==0 ) {

        IntImg * fimg = dynamic_cast<IntImg*>(image);
        int nullval = fimg->getNull();
        int* data = &*fimg->data().begin();
        fits_write_imgnull(fptr, TINT, 1, numPixel, 
            data, 
            fimg->nullDefined() ? &nullval : 0,
            &status);

    } else {
        throw std::invalid_argument(std::string("Attempt to write unsupported image type")+
            typeid(image).name());
    }
#endif
    return status;
}

