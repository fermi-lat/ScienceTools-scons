/** @file FitsService.h
@brief declaration of FitsService
*/
#ifndef FITS_SERVICE_H
#define FITS_SERVICE_H

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iostream>

#include "table/Constants.h"
#include "table/Exception.h"




namespace table{
#include "fitsio.h"

/** @class FitsService

*/
class FitsService //: virtual public table::IDataIOService
{
public:
    // Following typedefs are taken From CCfits - Author Ben Dorman
    typedef enum {Tnull, Tbit=TBIT, Tbyte=TBYTE, Tlogical=TLOGICAL, 
        Tstring=TSTRING, Tushort=TUSHORT, Tshort=TSHORT, Tuint=TUINT, Tint=TINT, 
        Tulong=TULONG, Tlong=TLONG, Tfloat=TFLOAT, Tdouble=TDOUBLE, Tcomplex=TCOMPLEX, 
        Tdblcomplex=TDBLCOMPLEX, VTbit=-TBIT, VTbyte=-TBYTE, VTlogical=-Tlogical, 
        VTushort=-TUSHORT, VTshort=-TSHORT, VTuint=-TUINT, VTint=-TINT, VTulong=-TULONG,
        VTlong=-TLONG, VTfloat=-TFLOAT, VTdouble=-TDOUBLE, VTcomplex=-TCOMPLEX,
        VTdblcomplex=-TDBLCOMPLEX} FitsValueType;
    typedef enum {Ibyte=BYTE_IMG, Ishort = SHORT_IMG, Ilong = LONG_IMG, Ifloat = FLOAT_IMG, 
        Idouble = DOUBLE_IMG, Iushort = USHORT_IMG, Iulong = ULONG_IMG} FitsImageType;



    // Constructors and destructors
    FitsService();
    FitsService(const std::vector<std::string> &fname, RWmode mode=Read);
    FitsService(const std::string &fname, const std::string &templateFile);

    virtual ~FitsService();


    // Helper Methods
    void swap(FitsService &other) throw();


    // File operations
    virtual const std::vector<std::string> &filename() const;
    virtual const std::string &currentFile() const;
    const fitsfile *fptr() const   { return m_fptr; }
    virtual const int mode() const;
    virtual const std::string &extname() const;
    virtual const long nrows(const std::string &extname) const;


    // Return mission and extension names
    virtual const std::string extname(const std::string &key, std::string &value) const;


    // Error Messages
    virtual void getErrorMessage(int status, std::string &message) const;
    virtual void clearErrorMsg();


    // Filter data
    virtual void filter(const std::string &extname, const std::string &expression);




    // Make specified extension current
    virtual void makeCurrent(const std::string &extname) const;
    virtual void makePrimaryCurrent() const;


    // Reset pointer to first filename
    virtual void resetFilePointer();

#if 0
    // Create copy of a table
    virtual void copyHeader();
    virtual void copyData(const std::string &key, const std::string &newName);
#endif

    //  Read Data
    virtual void read(const std::string &key, std::vector<std::string> &data, 
        long firstRow=1, long nrows=-1) const;
    virtual void read(const std::string &key, std::vector<double> &data, 
        long firstRow=1, long nrows=-1) const;
    virtual void read(const std::string &key, std::vector<float> &data, 
        long firstRow=1, long nrows=-1) const;
    virtual void read(const std::string &key, std::vector<long> &data, 
        long firstRow=1, long nrows=-1) const;
    virtual void read(const std::string &key, std::vector<int> &data, 
        long firstRow=1, long nrows=-1) const;
    virtual void read(const std::string &key, std::vector<short> &data, 
        long firstRow=1, long nrows=-1) const;



    // Read Image
    virtual void readImage(long firstElem, long nelements, std::vector<double> &data) const;
    virtual void readImage(long firstElem, long nelements, std::vector<float> &data) const;
    virtual void readImage(long firstElem, long nelements, std::vector<unsigned long> &data) const;
    virtual void readImage(long firstElem, long nelements, std::vector<long> &data) const;
    virtual void readImage(long firstElem, long nelements, std::vector<char> &data) const;
    virtual void readImage(long firstElem, long nelements, std::vector<unsigned short> &data) const;
    virtual void readImage(long firstElem, long nelements, std::vector<short> &data) const;


    virtual void readImage(const long *fpixel, const long *lpixel, const long *inc, 
        std::vector<double> &data) const;
    virtual void readImage(const long *fpixel, const long *lpixel, const long *inc, 
        std::vector<float> &data) const;
    virtual void readImage(const long *fpixel, const long *lpixel, const long *inc, 
        std::vector<long> &data) const;
    virtual void readImage(const long *fpixel, const long *lpixel, const long *inc, 
        std::vector<int> &data) const;
    virtual void readImage(const long *fpixel, const long *lpixel, const long *inc, 
        std::vector<short> &data) const;


    // Read Keys
    virtual void readKey(int keynum, std::string &key, std::string &value, std::string &comment) const;
    virtual void readKey(const std::string &key, std::string &value, std::string &comment) const;
    virtual void readKey(const std::string &key, float &value, std::string &comment) const;
    virtual void readKey(const std::string &key, long &value, std::string &comment) const;
    virtual void readKey(const std::string &key, int &value, std::string &comment) const;
    virtual void readKey(const std::string &key, bool &value, std::string &comment) const;
    void readStringKey(const std::string &key, std::string &value, std::string &comment) const;

    virtual void readImageHeader( int &bitpix, int &naxis,
        std::vector<long> &naxes, int &extend, double &zero, double &scale) const;
    virtual void writeImageHeader( int &bitpix, int &naxis,
        const std::vector<long> &naxes, bool &extend, double &zero, double &scale) const;
#if 0

    //  Write Data
    virtual void write(const std::string &key, const std::vector<std::string> &data, 
        long firsRow=1) const;
    virtual void write(const std::string &key, const std::vector<double> &data, 
        long firsRow=1) const;
    virtual void write(const std::string &key, const std::vector<float> &data, 
        long firsRow=1) const;
    virtual void write(const std::string &key, const std::vector<long> &data, 
        long firsRow=1) const;
    virtual void write(const std::string &key, const std::vector<int> &data, 
        long firsRow=1) const;
    virtual void write(const std::string &key, const std::vector<short> &data, 
        long firsRow=1) const;

#endif


    // Write Image
    virtual void writeImage(long firstElem, long nelements, const std::vector<double> &data) const;
    virtual void writeImage(long firstElem, long nelements, const std::vector<float> &data) const;
    virtual void writeImage(long firstElem, long nelements, const std::vector<unsigned long> &data) const;
    virtual void writeImage(long firstElem, long nelements, const std::vector<long> &data) const;
    virtual void writeImage(long firstElem, long nelements, const std::vector<char> &data) const;
    virtual void writeImage(long firstElem, long nelements, const std::vector<unsigned short> &data) const;
    virtual void writeImage(long firstElem, long nelements, const std::vector<short> &data) const;

    virtual void writeImage(const long *fpixel, const long *lpixel, 
        const std::vector<double> &data) const;
    virtual void writeImage(const long *fpixel, const long *lpixel, 
        const std::vector<float> &data) const;
    virtual void writeImage(const long *fpixel, const long *lpixel, 
        const std::vector<long> &data) const;
    //virtual void writeImage(const long *fpixel, const long *lpixel, 
    //	const std::vector<int> &data) const;
    virtual void writeImage(const long *fpixel, const long *lpixel, 
        const std::vector<short> &data) const;


    // Write Keys
    virtual void writeKey(const std::string &key, const std::string &value, 
        const std::string &comment=std::string("")) const;
    virtual void writeKey(const std::string &key, double value, 
        const std::string &comment=std::string("")) const;
    virtual void writeKey(const std::string &key, float value, 
        const std::string &comment=std::string("")) const;
    virtual void writeKey(const std::string &key, long value, 
        const std::string &comment=std::string("")) const;
    virtual void writeKey(const std::string &key, int value, 
        const std::string &comment=std::string("")) const;
    virtual void writeKey(const std::string &key, short value, 
        const std::string &comment=std::string("")) const;
    virtual void writeKey(const std::string &key, bool value, 
        const std::string &comment=std::string("")) const;
    void writeStringKey(const std::string &key, const std::string &value, 
        const std::string &comment) const;


    // Delete data
    virtual void deleteKey(const std::string &key);
    virtual void deleteData(const std::string &key);
    virtual void nextKey(const std::string &key, std::string &value, 
        std::string &comment) const;
    virtual void writeHistory(const std::string &history) const;
    virtual void writeComment(const std::string &comment);

    virtual void closeFile();
    virtual void createNewFile(const std::string &fname, const std::string &templateFile);
    virtual void createFile(const std::string &fname);

    virtual char keyType(const std::string &value) const;
    virtual bool keyClassOK(const std::string &key) const;
    virtual int numKeys() const;

private:    
    // Data Members
    std::vector<std::string>                  m_filename;
    std::vector<std::string>::const_iterator  m_it;
    fitsfile                                  *m_fptr;
    mutable std::string                       m_currentExtension;
    RWmode                                    m_mode;
    int                                       m_numExtensions;
    std::map<std::string, long>               m_numRows;
    std::map<std::string, int>                m_numKeys;


    enum {STRLEN=72};


    // Helper Methods

    fitsfile *copyFitsPtr(const fitsfile *inPtr);   // copied from Ben Dorman's CCFits::FITSUtil.cxx::copyFitsPtr method
    void createNewFilePtr(const std::string &fname, RWmode mode);
    virtual void openFile(const std::string &fname);
    virtual void openFile(const std::string &fname, fitsfile **fptr);
    virtual void doFinalTouchups();
    void generateExtensionPars();
    int columnNumber(const std::string &columnName) const;
    FitsValueType columnType(int colnum, long &repeat) const;
    long variableColumnType(int colnum, long rownum) const;
    virtual long rowsToRead(long firstRow, long nrows) const;
    //virtual long rowsToWrite(long nrows, const size_t sizeOfData) const;
    void modifyVectorLength(int colnum, long len) const;
    virtual const std::string phaType() const;
    virtual void resetBitpix(int &bitpix) const;
    virtual void resetBitpixToWrite(int &bitpix) const;
    void getNumHdus();


    // Return true if there is another file to read
    virtual const bool next();



    //int FitsService::numHdus() const;
    std::string findAndReadKey(const std::string &key, const std::string &value="", 
        const std::string &keyToRead="") const;


    template<typename T>
        void readData(const std::string &key, std::vector<T> &data, long firstRow=1, 
        long nrows=-1) const;

#ifdef SPEC_TEMPLATE_DECL_DEFECT
    template<>
        void readData<std::string>(const std::string &key, std::vector<std::string> &data, 
        long firstRow, long nrows) const;
#endif


    template<typename T>
        void readColumn(const FitsValueType datatype, int colnum, long firstRow, 
        long firstElem, long nelements, T *array) const;

    template<typename T>
        void readImage(long firstElem, long nelements, std::vector<T> &data) const;


    template<typename T>
        void readImage(const long *fpixel, const long *lpixel, const long *inc, 
        std::vector<T> &data) const;

    template<typename T>
        void readKey(const std::string &key, const FitsValueType datatype, T &value, 
        std::string &comment) const;

    template<typename T>
        void writeData(const std::string &key, const std::vector<T> &data, long firstRow) const;

#ifdef SPEC_TEMPLATE_DECL_DEFECT
    template<>
        void writeData<std::string>(const std::string &key, const std::vector<std::string> &data, 
        long firstRow) const;
#endif

    template<typename T>
        void writeColumn(const FitsValueType datatype, int colnum, long firstRow, 
        long firstElem, long nelements, const T *array) const;

    template<typename T>
        void writeImage(const FitsValueType datatype, long firstElem, long nelements,
        const std::vector<T> &data) const;

    template<typename T>
        void writeImage(const FitsValueType datatype, const long *fpixel, const long *lpixel, 
        const std::vector<T> &data) const;

    template<typename T>
        void writeKey(const std::string &key, const FitsValueType datatype, const T &value, 
        const std::string &comment=std::string("")) const;

    //! throw an exception with a detailed message if an error
    void report_error(int status)const;


    // Taken from CCfits::FitsUtil.h written by Ben Dorman.
    template <typename T>
    struct MatchType 
    {
        FitsValueType operator () ()
        {
            if (typeid(T) == typeid(std::string)) return Tstring;
            if (typeid(T) == typeid(double)) return Tdouble;
            if (typeid(T) == typeid(float)) return Tfloat;
            if (typeid(T) == typeid(long)) return Tlong;
            if (typeid(T) == typeid(unsigned long)) return Tulong;
            if (typeid(T) == typeid(int)) return Tint;
            if (typeid(T) == typeid(unsigned int)) return Tuint;
            if (typeid(T) == typeid(short)) return Tshort;
            if (typeid(T) == typeid(unsigned short)) return Tushort;
            if (typeid(T) == typeid(bool)) return Tlogical;
            if (typeid(T) == typeid(unsigned char)) return Tbyte;

            return FitsService::Tnull;
        }
    };
};


template<typename T>
void FitsService::readKey(const std::string &key, const FitsValueType datatype, T &value, 
                          std::string &comment) const
{ 
    int status=Success;
    char comm[FLEN_COMMENT];
    strcpy(comm, "   ");
    if (fits_read_key(m_fptr, datatype, const_cast<char *>(key.c_str()), &value, comm, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::readKey:\nError while reading keyword " << 
            key << ".\n";
        throw Exception(errorMessage.str());
    }

    comment = std::string(comm);
}


template<typename T>
void FitsService::readData(const std::string &key, std::vector<T> &data, 
                           long firstRow, long nrows) const
{  
    try
    {
        long numRowsToRead = rowsToRead(firstRow, nrows);
        int colnum = columnNumber(key);

        MatchType<T> dtype;
        FitsValueType datatype = dtype();

        T *array = new T[numRowsToRead];
        readColumn(datatype, colnum, firstRow, 1, numRowsToRead, array);

        data.resize(numRowsToRead);
        std::copy(&array[0], &array[numRowsToRead], data.begin());

        delete [] array;
    }

    catch(...)
    {
        throw Exception("FitsService::readData:Error while reading "+key+".\n");
    }
}








template<typename T>
void FitsService::readColumn(const FitsValueType datatype, int colnum, long firstRow, 
                             long firstElem, long nelements, T *array) const
{
    int status=Success;
    T nulval;
    int anynul=0;
    if (fits_read_col(m_fptr, datatype, colnum, firstRow, firstElem, nelements, &nulval, array, &anynul, 
        &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::readColumn:\n" <<
            "Error while reading column#: " << colnum << ".\n";
        throw Exception(errorMessage.str());
    }
}


template<typename T>
void FitsService::readImage(long firstElem, long nelements, std::vector<T> &data) const
{
        int status=Success;

        MatchType<T> dtype;
        FitsValueType datatype = dtype();

        int anynul=0;
        T nulval;
        if (fits_read_img(m_fptr, datatype, firstElem, nelements, &nulval, 
            &*data.begin(), &anynul, &status))
        {
            report_error(status);
        }
}



template<typename T>
void FitsService::readImage(const long *fpixel, const long *lpixel, const long *inc, 
                            std::vector<T> &data) const
{
    try
    {
        int status=Success;

        MatchType<T> dtype;
        FitsValueType datatype = dtype();

        int anynul=0;
        size_t sz = data.size();
        T *array = new T[sz];
        T nulval;
        if (fits_read_subset(m_fptr, datatype, const_cast<long *>(fpixel), const_cast<long *>(lpixel),
            const_cast<long *>(inc), &nulval, array, &anynul, &status))
        {
            delete [] array;
            report_error(status);
        }

        data.resize(sz);
        std::copy(&array[0], &array[sz], data.begin());
        delete [] array;
    }

    catch(...)
    {
        throw Exception("FitsService::readImage:\nError while reading image.\n");
    }
}




template<typename T>
void FitsService::writeData(const std::string &key, const std::vector<T> &data, long firstRow) const
{
    try
    {
        int colnum = columnNumber(key);

        long repeat;
        FitsValueType datatype = columnType(colnum, repeat);

        const size_t sz=data.size();
        switch(datatype)
        {
        case Tdouble:
        case VTdouble:
            {
                double *array = new double[sz];
                std::copy(data.begin(), data.end(), &array[0]);
                writeColumn(Tdouble, colnum, firstRow, 1L, data.size(), array);
                delete [] array;
                break;
            }

        case Tfloat:
        case VTfloat:
            {
                float *array = new float[sz];
                std::copy(data.begin(), data.end(), &array[0]);
                writeColumn(Tfloat, colnum, firstRow, 1L, data.size(), array);
                delete [] array;
                break;
            }

        case Tlong:
        case VTlong:
            {
                long *array = new long[sz];
                std::copy(data.begin(), data.end(), &array[0]);
                writeColumn(Tlong, colnum, firstRow, 1L, data.size(), array);
                delete [] array;
                break;
            }

        case Tint:
        case VTint:
            {
                int *array = new int[sz];
                std::copy(data.begin(), data.end(), &array[0]);
                writeColumn(Tint, colnum, firstRow, 1L, data.size(), array);
                delete [] array;
                break;
            }

        case Tshort:
        case VTshort:
            {
                short *array = new short[sz];
                std::copy(data.begin(), data.end(), &array[0]);
                writeColumn(Tshort, colnum, firstRow, 1L, data.size(), array);
                delete [] array;
                break;
            }

        case Tbyte:
        case VTbyte:
            {
                char *array = new char[sz];
                std::copy(data.begin(), data.end(), &array[0]);
                writeColumn(Tbyte, colnum, firstRow, 1L, data.size(), array);
                delete [] array;
                break;
            }

        default:
            {
                std::ostringstream errorMessage;
                errorMessage << "FitsService::writeData:\n" <<
                    "Invalid data type for FITS Data I/O.\n";
                throw Exception(errorMessage.str());
            }
        }
    }

    catch(...)
    {
        throw Exception(
            "FitsService::writeData:\nError while writing "+key+".\n");
    }
}








template<typename T>
void FitsService::writeColumn(const FitsValueType datatype, int colnum,  long firstRow, 
                              long firstElem, long nelements, const T *array) const
{
    int status=Success;

    if (fits_write_col(m_fptr, datatype, colnum, firstRow, firstElem, nelements, 
        const_cast<T *>(array), &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::writeColumn:\nError while writing Collumn#: " <<
            colnum << ".\n";
        throw Exception(errorMessage.str());
    }
}


template<typename T>
void FitsService::writeImage(const FitsValueType datatype, long firstElem, long nelements, 
                             const std::vector<T> &data) const
{
    try
    {
        int status=Success;
        size_t sz = data.size();

        if (sz > 0)
        {
            T *array = new T[sz];
            std::copy(data.begin(), data.end(), array);
            if (fits_write_img(m_fptr, datatype, firstElem, nelements, array, &status))
            {
                delete [] array;
                std::ostringstream errorMessage;
                errorMessage << "FitsService::writeImage:\n" <<
                    "Error while writing image.\n";
                throw Exception(errorMessage.str());
            }

            delete [] array;
        }
    }

    catch(...)
    {
        throw Exception("FitsService::writeImage:\nError while writing image.\n");
    }
}




template<typename T>
void FitsService::writeImage(const FitsValueType datatype, const long *fpixel, const long *lpixel, 
                             const std::vector<T> &data) const
{
    try
    {
        int status=Success;
        size_t sz = data.size();

        if (sz > 0)
        {
            T *array = new T[sz];
            std::copy(data.begin(), data.end(), array);
            if (fits_write_subset(m_fptr, datatype, const_cast<long *>(fpixel), const_cast<long *>(lpixel),
                array, &status))
            {
                delete [] array;
                std::ostringstream errorMessage;
                errorMessage << "FitsService::writeImage:\n" <<
                    "Error while writing image.\n";
                throw Exception(errorMessage.str());
            }

            delete [] array;
        }
    }

    catch(...)
    {
        throw Exception("FitsService::writeImage:\nError while writing image.\n");
    }
}



template<typename T>
void FitsService::writeKey(const std::string &key, const FitsValueType datatype, const T &value, 
                           const std::string &comment) const
{ 
    int status=Success;

    char *comm=0;
    //char comm[FLEN_COMMENT]="";
    if (comment.size() > 0)
    {
        comm = new char[comment.size()+1];
        strcpy(comm, comment.c_str());
    }

    T val = value;
    //if (fits_update_key(m_fptr, datatype, const_cast<char *>(key.c_str()), const_cast<T *>(&value), 
    if (fits_update_key(m_fptr, datatype, const_cast<char *>(key.c_str()), &val, 
        comm, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::writeKey:\n" <<
            "Error while writing key: " << key << ".\n";
        throw Exception(errorMessage.str());
    }

    if (comm)
        delete comm;
}




// Inline methods
inline const std::vector<std::string> &FitsService::filename() const
{	
    return m_filename; 
}


inline const std::string &FitsService::currentFile() const
{	
    return *m_it; 
}


inline const int FitsService::mode() const
{	
    return m_mode; 
}


inline const std::string &FitsService::extname() const
{	
    return m_currentExtension; 
}


template<>
inline void FitsService::readData<std::string>(const std::string &key, std::vector<std::string> &data, 
                                               long firstRow, long nrows) const
{
    try
    {
        long numRowsToRead = rowsToRead(firstRow, nrows);
        int colnum = columnNumber(key);
        long repeat=0L;
        FitsValueType datatype = columnType(colnum, repeat);

        if (datatype == Tstring)
        {
            int status=Success;
            int anynul=0;
            long firstElem=1;

            char **array = new char*[numRowsToRead];
            for (long i=0; i<numRowsToRead; ++i)
                array[i] = new char[STRLEN];

            char *nulval=0;
            if (!fits_read_col_str(m_fptr, colnum, firstRow, firstElem, numRowsToRead, 
                nulval, array, &anynul, &status))
            {
                data.resize(numRowsToRead);
                std::copy(&array[0], &array[numRowsToRead], data.begin());
            }

            delete []array;

            if (status != Success)
            {
                std::string message;
                getErrorMessage(status, message);
                throw Exception(message);
            }
        }

        else
            throw "Invalid type";
    }

    catch(...)
    {
        throw Exception("FitsService::readData:\nError while reading "+key+".\n");
    }
}


template<>
inline void FitsService::writeData<std::string>(const std::string &key, 
                                                const std::vector<std::string> &data, long firstRow) const
{
    try
    {
        int colnum = columnNumber(key);

        long repeat;
        FitsValueType datatype = columnType(colnum, repeat);

        const size_t sz=data.size();
        switch(datatype)
        {
        case Tstring:
            {
                char **array = new char*[sz];
                for (size_t i=0; i<sz; ++i)
                {
                    array[i] = new char[STRLEN];
                    strcpy(array[i], const_cast<char *>(data[i].c_str()));
                }
                writeColumn(datatype, colnum, firstRow, 1L, data.size(), array);
                delete [] array;
                break;
            }

        default:
            {
                std::ostringstream errorMessage;
                errorMessage << "FitsService::writeData<std::string>:\n" <<
                    "Invalid FITS data type.\n";
                throw Exception(errorMessage.str());
            }
        }
    }

    catch(...)
    {
        throw Exception(
            "FitsService::writeData:\nError while writing "+key+".\n");
    }
}

} // namespace table
#endif //FITS_SERVICE_H
