/** @file FitsService.cxx
    @brief implementation of FitsService
*/

#include "table/Exception.h"
#include "table/FitsService.h"
#include "facilities/Util.h" // for expand env var

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace
{
    const std::string primary="PRIMARY";
    const long MAXDIM=99;

    void toUpper(std::string &str) {
        size_t strSize=str.size();
        for (size_t i=0; i<strSize; ++i)
            str[i] = toupper(str[i]);
    }

}

using namespace table;

FitsService::FitsService()
: m_filename(),
m_it(),
m_fptr(0),
m_currentExtension(),
m_mode(Write),
//m_fileFmt(Fits),
m_numExtensions(0),
m_numRows()
{
}



FitsService::FitsService(const std::vector<std::string> &fname, RWmode mode)
: m_filename(fname),
m_it(m_filename.begin()),
m_fptr(0),
m_currentExtension(),
m_mode(mode),
//m_fileFmt(Fits),
m_numExtensions(0),
m_numRows()
{
    createNewFilePtr(*(fname.begin()), mode);
}



FitsService::FitsService(const std::string &fname, const std::string &templateFile)
: m_fptr(0),
m_currentExtension(),
m_mode(Write),
//m_fileFmt(Fits),
m_numExtensions(0),
m_numRows()
{
    createNewFile(fname, templateFile);
}


FitsService::~FitsService()
{
    closeFile(); 
}




void FitsService::createNewFilePtr(const std::string &fname, RWmode mode)
{
    switch(mode)
    {
    case Read:
        openFile(fname);
        generateExtensionPars();
        break;

    case Write:  
        createFile(fname);
        break;

    default:
        {
            std::ostringstream errorMessage;
            errorMessage << "RootService::createNewFilePtr:\nInvalid File Mode.\n";
            throw Exception(errorMessage.str());
        }
    }
}



void FitsService::openFile(const std::string &fname)
{
    int status=Success;

    if (fits_open_file(&m_fptr, fname.c_str(), m_mode, &status))
    {
        if (status == FILE_NOT_OPENED) 
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::openFile:\nCannot open file - " <<
                fname << ".\n";
            throw Exception(errorMessage.str());
        }
        else 
        {
            std::string message;
            getErrorMessage(status, message);
            std::ostringstream errorMessage;
            errorMessage << "FitsService::openFile:\n" << message << "\n";
            throw Exception(errorMessage.str());
        }
    }
}


void FitsService::openFile(const std::string &fname, fitsfile **fptr)
{
    int status=Success;

    if (fits_open_file(fptr, fname.c_str(), m_mode, &status))
    {
        if (status == FILE_NOT_OPENED) 
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::openFile:\nCannot open file - " <<
                fname << ".\n";
            throw Exception(errorMessage.str());
        }

        else 
        {
            std::string message;
            getErrorMessage(status, message);
            std::ostringstream errorMessage;
            errorMessage << "FitsService::openFile:\n" << message << "\n";
            throw Exception(errorMessage.str());
        }
    }
}


void FitsService::createFile(const std::string &fname)
{
    int status=Success;

    std::string filename = fname;

    if (fits_create_file(&m_fptr, const_cast<char *>(filename.c_str()), &status))
    {
        if (status == FILE_NOT_CREATED) 
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::createFile:\nCannot create file - " <<
                fname << ".\n";
            throw Exception(errorMessage.str());
        }

        else 
        {
            std::string message;
            getErrorMessage(status, message);
            throw Exception(message);
        }
    }

    //THB: leave this at zero: it seems to set the number of extensions to 1. 
    // getNumHdus();
}


void FitsService::createNewFile(const std::string &fname, const std::string &templateFile)
{
    m_filename.clear();   
    m_filename.push_back(fname);
    std::string tfile(templateFile);
    facilities::Util::expandEnvVar(&tfile);

    m_it = &m_filename.back();

    facilities::Util::expandEnvVar(& m_filename.back());

    std::string oname = *m_it + "(" + tfile + ")";
    createFile(oname);
}


const long FitsService::nrows(const std::string &extname) const
{
    std::string temp = extname;
    toUpper(temp);
    std::map<std::string, long>::const_iterator it = m_numRows.find(temp);
    if (it != m_numRows.end())
        return (*(m_numRows.find(temp))).second;

    else
    {
        int status = Success;

        long nrows=0;
        status = fits_movnam_hdu(m_fptr, ANY_HDU, const_cast<char *>(extname.c_str()), 0, &status);
        status = fits_get_num_rows(m_fptr, &nrows, &status);

        if (status == Success)
            return nrows;

        else
        {
            std::string message;
            getErrorMessage(status, message);
            std::ostringstream errorMessage;
            errorMessage << "FitsService::nrows:\n" << message << ".\n";
            throw Exception(errorMessage.str());
        }
    }
}



void FitsService::generateExtensionPars()
{
    int status = Success;
    getNumHdus();

    int hdutype=0;
    std::string extname("EXTNAME");
    long nrows=0;
    int nkeys=0;
    int moreKeys=0;
    char val[FLEN_VALUE];
    char comment[FLEN_COMMENT];
    for (int i=1; i<=m_numExtensions && status==Success; ++i)
    {
        status = fits_movabs_hdu(m_fptr, i, &hdutype, &status);

        if (i > 1)   // only for non-primary extensions
        {
            status = fits_read_key(m_fptr, Tstring, const_cast<char *>(extname.c_str()), val, comment, &status);
            status = fits_get_num_rows(m_fptr, &nrows, &status);
        }

        status = fits_get_hdrspace(m_fptr, &nkeys, &moreKeys, &status);

        if (status == Success)
        {
            std::string nm;
            if (i == 1)
                nm = primary;
            else
                nm = std::string(val);

            std::string temp=nm;
            toUpper(temp);
            m_numRows[temp] = nrows;
            m_numKeys[temp] = nkeys;
        }
    }

    if (status != Success)
    {
        std::string message;
        getErrorMessage(status, message);
        std::ostringstream errorMessage;
        errorMessage << "FitsService::generateExtensionPars:\n" << message << ".\n";
        throw Exception(errorMessage.str());
    }
}


// This code has been taken from CCfits::FITSUtils - Author: Ben Dorman
fitsfile *FitsService::copyFitsPtr(const fitsfile *inPtr)
{
    // allocate all the memory and convert any errors to a standard exception    

    fitsfile* outPtr = 0;
    if (inPtr->Fptr->open_count > 0)
    {
        openFile(inPtr->Fptr->filename, &outPtr);
        outPtr->Fptr->open_count = 1;
    }

    else
        if ( (outPtr  = (fitsfile*)calloc(1,sizeof(fitsfile))) == 0 ) throw std::bad_alloc();

    outPtr->Fptr = 0;
    if ( (outPtr->Fptr = (FITSfile*)calloc(1,sizeof(FITSfile))) == 0 ) throw std::bad_alloc();

    outPtr->Fptr->filename = 0;
    if ( (outPtr->Fptr->filename = 
        (char*)malloc(strlen(inPtr->Fptr->filename)+1))  == 0 ) throw std::bad_alloc();

    outPtr->Fptr->tableptr = 0;
    /* pointer to the table structure */
    if ( (outPtr->Fptr->tableptr  = (tcolumn*)malloc(sizeof(tcolumn))) == 0 )
        throw std::bad_alloc();

    outPtr->HDUposition = inPtr->HDUposition;

    outPtr->Fptr->filehandle = inPtr->Fptr->filehandle;
    /* defines which set of I/O drivers should be used */
    outPtr->Fptr->driver = inPtr->Fptr->driver;  

    strcpy(outPtr->Fptr->filename,inPtr->Fptr->filename);

    /* current size of the physical disk file in bytes */
    outPtr->Fptr->filesize = inPtr->Fptr->filesize;   

    /* logical size of file, including unflushed buffers */
    outPtr->Fptr->logfilesize = inPtr->Fptr->logfilesize; 

    /* 0 = readonly, 1 = readwrite */
    outPtr->Fptr->writemode = inPtr->Fptr->writemode;  

    /* byte offset in file to beginning of next keyword */
    outPtr->Fptr->datastart = inPtr->Fptr->datastart;

    /* current I/O pointer position in the physical file */
    outPtr->Fptr->curbuf = inPtr->Fptr->curbuf;

    /* number of opened 'fitsfiles' using this structure */  
    //outPtr->Fptr->open_count = 0; 
    //outPtr->Fptr->open_count = inPtr->Fptr->open_count; 
    //if (inPtr->Fptr->open_count > 0)
    //    openFile(outPtr->Fptr->filename, &outPtr);

    /* magic value used to verify that structure is valid */
    outPtr->Fptr->validcode = inPtr->Fptr->validcode;  

    outPtr->Fptr->lasthdu = inPtr->Fptr->lasthdu;    
    /* is this the last HDU in the file? 0 = no, else yes */
    outPtr->Fptr->bytepos = inPtr->Fptr->bytepos;   /* current logical I/O pointer position in file */
    outPtr->Fptr->io_pos = inPtr->Fptr->io_pos;    
    /* number of I/O buffer currently in use */ 
    outPtr->Fptr->curhdu = inPtr->Fptr->curhdu;     
    /* current HDU number; 0 = primary array */
    outPtr->Fptr->hdutype = inPtr->Fptr->hdutype;    
    /* 0 = primary array, 1 = ASCII table, 2 = binary table */
    outPtr->Fptr->maxhdu = inPtr->Fptr->maxhdu;     
    /* highest numbered HDU known to exist in the file */
    size_t i = 0;
    //if ( (outPtr->Fptr->headstart  = (long *)malloc(MAX_COMPRESS_DIM*sizeof(long))) == 0 )
    if ( (outPtr->Fptr->headstart  = (OFF_T*)malloc(MAX_COMPRESS_DIM*sizeof(OFF_T))) == 0 )
        throw std::bad_alloc();
    for ( ; i < MAX_COMPRESS_DIM; i++) 
    {
        outPtr->Fptr->headstart[i] = inPtr->Fptr->headstart[i];
    }
    /* byte offset in file to start of each HDU */
    outPtr->Fptr->headend = inPtr->Fptr->headend;   
    /* byte offest in file to end of the current HDU header */
    outPtr->Fptr->nextkey = inPtr->Fptr->nextkey;   
    /* byte offset in file to start of the current data unit */
    outPtr->Fptr->tfield = inPtr->Fptr->tfield;     
    /* number of fields in the table (primary array has 2 */
    outPtr->Fptr->origrows = inPtr->Fptr->origrows;  
    /* original number of rows (value of NAXIS2 keyword)  */
    outPtr->Fptr->numrows = inPtr->Fptr->numrows;   
    /* number of rows in the table (dynamically updated) */
    outPtr->Fptr->rowlength = inPtr->Fptr->rowlength; 
    /* total length of a table row, in bytes */

    outPtr->Fptr->heapstart = inPtr->Fptr->heapstart; 
    /* heap start byte relative to start of data unit */
    outPtr->Fptr->heapsize = inPtr->Fptr->heapsize;  /* size of the heap, in bytes */

    /* the following elements are related to compress images */
    /* 1 if HDU contains a compressed image, else 0 */
    outPtr->Fptr->compressimg = inPtr->Fptr->compressimg; 

    /* compression type string */
    for ( i = 0; i < 12; ++i)
    {
        outPtr->Fptr->zcmptype[i] = inPtr->Fptr->zcmptype[i];
    }
    /* type of compression algorithm */
    outPtr->Fptr->compress_type = inPtr->Fptr->compress_type;      
    /* FITS data type of image (BITPIX) */
    outPtr->Fptr->zbitpix = inPtr->Fptr->zbitpix;		
    /* dimension of image */
    outPtr->Fptr->zndim = inPtr->Fptr->zndim;		

    /* length of each axis */
    for ( i = 0; i < MAX_COMPRESS_DIM; i++) 
    {
        outPtr->Fptr->znaxis[i] = inPtr->Fptr->znaxis[i];
    }
    /* size of compression tiles */
    for ( i = 0; i < MAX_COMPRESS_DIM; i++) 
    {
        outPtr->Fptr->tilesize[i] = inPtr->Fptr->tilesize[i];
    }
    /* max number of pixels in each image tile */
    outPtr->Fptr->maxtilelen = inPtr->Fptr->maxtilelen;        
    /* maximum length of variable length arrays */
    outPtr->Fptr->maxelem = inPtr->Fptr->maxelem;		

    /* column number for COMPRESSED_DATA column */
    outPtr->Fptr->cn_compressed = inPtr->Fptr->cn_compressed;	    
    /* column number for UNCOMPRESSED_DATA column */
    outPtr->Fptr->cn_uncompressed = inPtr->Fptr->cn_uncompressed;    
    /* column number for ZSCALE column */
    outPtr->Fptr->cn_zscale = inPtr->Fptr->cn_zscale;	    
    /* column number for ZZERO column */
    outPtr->Fptr->cn_zzero = inPtr->Fptr->cn_zzero;	    
    /* column number for the ZBLANK column */
    outPtr->Fptr->cn_zblank = inPtr->Fptr->cn_zblank;          

    /* scaling value, if same for all tiles */
    outPtr->Fptr->zscale = inPtr->Fptr->zscale;          
    /* zero pt, if same for all tiles */
    outPtr->Fptr->zzero = inPtr->Fptr->zzero;           
    /* value for null pixels, if not a column */
    outPtr->Fptr->zblank = inPtr->Fptr->zblank;             

    /* first compression parameter */
    outPtr->Fptr->rice_blocksize = inPtr->Fptr->rice_blocksize;     
    /* second compression parameter */
    outPtr->Fptr->rice_nbits = inPtr->Fptr->rice_nbits;         


    /* column name = FITS TTYPEn keyword; */
    if (inPtr->Fptr->tableptr)
    {
        if (inPtr->Fptr->tableptr->ttype)
        {
            for ( i = 0; i < 70; ++i)
            {
                outPtr->Fptr->tableptr->ttype[i] = inPtr->Fptr->tableptr->ttype[i];
            }

            /* FITS null value string for ASCII table columns */
            if (inPtr->Fptr->tableptr->strnull)
            {
                for ( i = 0; i < 20; ++i)
                {
                    outPtr->Fptr->tableptr->strnull[i] = inPtr->Fptr->tableptr->strnull[i];
                }
            }

            /* FITS tform keyword value  */
            if (inPtr->Fptr->tableptr->tform)
            {
                for ( i = 0; i < 10; ++i)
                {
                    outPtr->Fptr->tableptr->tform[i] = inPtr->Fptr->tableptr->tform[i];
                }
            }
        }

        /* offset in row to first byte of each column */
        outPtr->Fptr->tableptr->tbcol = inPtr->Fptr->tableptr->tbcol;       
        /* datatype code of each column */
        outPtr->Fptr->tableptr->tdatatype = inPtr->Fptr->tableptr->tdatatype;   
        /* repeat count of column; number of elements */
        outPtr->Fptr->tableptr->trepeat = inPtr->Fptr->tableptr->trepeat;     
        /* FITS TSCALn linear scaling factor */
        outPtr->Fptr->tableptr->tscale = inPtr->Fptr->tableptr->tscale;    
        /* FITS TZEROn linear scaling zero point */
        outPtr->Fptr->tableptr->tzero = inPtr->Fptr->tableptr->tzero;     
        /* FITS null value for int image or binary table cols */
        outPtr->Fptr->tableptr->tnull = inPtr->Fptr->tableptr->tnull;       
        /* width of each ASCII table column */
        outPtr->Fptr->tableptr->twidth = inPtr->Fptr->tableptr->twidth;     
    }

    return outPtr;

}      




int FitsService::numKeys() const
{
    try
    {
        return (*(m_numKeys.find(m_currentExtension))).second;
    }

    catch(...)
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::numKeys:\nExtension " << m_currentExtension <<
            " does not exist in specified file.\n";
        throw Exception(errorMessage.str());
    }
}


void FitsService::nextKey(const std::string &key, std::string &value,
                          std::string &comment) const
{
    // Find a key containing specified keyword
    char **inclist=new char*[1];
    char **exclist=NULL;
    inclist[0] = new char[FLEN_KEYWORD];
    strcpy(inclist[0], key.c_str());

    char *card = new char[FLEN_CARD];
    char *comm = new char[FLEN_COMMENT];
    char *val   = new char[FLEN_VALUE];
    int status=Success;

    status = fits_find_nextkey(m_fptr, inclist, 1, exclist, 0, card, &status);
    status = fits_parse_value(card, val, comm, &status);
    value = val;
    comment = comm;

    delete [] val;
    delete [] comm;
    delete [] inclist[0];
    delete [] inclist;
    delete [] card;
}


std::string FitsService::findAndReadKey(const std::string &key, const std::string &value, 
                                        const std::string &keyToRead) const
{
    // Find a key containing specified keyword
    char **inclist=new char*[1];
    char **exclist=NULL;
    inclist[0] = new char[FLEN_KEYWORD];
    strcpy(inclist[0], key.c_str());

    int hdutype=0;
    char *card = new char[FLEN_CARD];
    char *keyname = new char[FLEN_KEYWORD];
    char *comment = new char[FLEN_COMMENT];
    char *temp    = new char[FLEN_VALUE];
    int status=Success;
    bool found=false;

    for (int i=0; i<m_numExtensions && !found; ++i)
    {
        status = Success;
        status = fits_movabs_hdu(m_fptr, i+1, &hdutype, &status);
        //status = fits_set_hdustruc(m_fptr, &status);
        //status = fits_find_nextkey(m_fptr, inclist, 1, exclist, 0, card, &status);
        //status = fits_parse_value(card, temp, comment, &status);
        //status = 0;
        status = fits_read_key(m_fptr, Tstring, inclist[0], temp, comment, &status);
        //if (status == Success)
        //	found = true;
        if ((status == Success) &&
            ((value == "") ||
            ((value != "") && (std::string(temp).find(value) != std::string::npos))))
            found = true;
    }

    delete [] inclist[0];
    delete [] inclist;
    delete [] card;
    delete [] keyname;

    std::string returnValue;

    if ((!found) || (status != Success))
    {
        //std::string message;
        //getErrorMessage(status, message);
        //std::string keyerror = "Error while reading/locating specified key/value pair (key: " +
        //	key + ") from file: " + *m_it;

        std::ostringstream errorMessage;
        errorMessage << "FitsService::findAndReadKey:\nError while reading/" <<
            "locating specified key/value pair (key: " << key << ") from file: " <<
            *m_it;
        throw Exception(errorMessage.str());
        //throw KeywordError(keyerror, message, true, false);
    }

    if ((status == Success) && (keyToRead != "") && (keyToRead != key))
    {
        if (value != "")
            status = fits_read_key(m_fptr, Tstring, const_cast<char *>(keyToRead.c_str()), 
            temp, comment, &status);
    }

    delete [] comment;

    if (status == Success)
    {
        returnValue = std::string(temp);
        delete [] temp;
        return returnValue;
    }

    else
    {
        delete [] temp;
        std::ostringstream errorMessage;
        errorMessage << "FitsService::findAndReadKey:\n" << 
            "Error while reading keyword " << std::string(inclist[0]) << "\n";
        throw Exception(errorMessage.str());
    }
}




const std::string FitsService::phaType() const
{
    std::string key="HDUCLAS4";
    return findAndReadKey(key);
}


const std::string FitsService::extname(const std::string &key, std::string &value) const   
{
    std::string tempkey=std::string("TELESCOP");
    std::string mission = findAndReadKey(tempkey);
    if (mission == std::string("CGRO"))   // to accomodate EGRET where INSTRUME contains the mission name
    {
        return findAndReadKey(std::string("EXTNAME"));
    }
    else {
        return findAndReadKey(key, value, std::string("EXTNAME"));
    }
}


void FitsService::getNumHdus()
{
    int status=Success;
    if (fits_get_num_hdus(m_fptr, &m_numExtensions, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::getNumHdus:\nError while reading " <<
            "number of extensions from the file.\n";
        throw Exception(errorMessage.str());
    }
}


const bool FitsService::next() 
{
    if (++m_it == m_filename.end())
    {
        return false;
    }

    else
    {
        closeFile();
        m_currentExtension = "";
        createNewFilePtr(*m_it, m_mode);

        return true;
    }
}


void FitsService::filter(const std::string &extname, const std::string &expression)
{
    std::string inputFile = *m_it + "[" + extname + "][" + expression + "]";

    int status=Success;
    closeFile();
    status = fits_open_file(&m_fptr, const_cast<char *>(inputFile.c_str()), Read, &status);

    //status = fits_get_num_hdus(m_fptr, &m_numExtensions, &status);
    getNumHdus();

    long nrows=0;
    status = fits_get_num_rows(m_fptr, &nrows, &status);

    if (status == Success)
    {
        std::string temp=extname;
        toUpper(temp);
        m_numRows[temp] = nrows;
    }

    if (status != Success) 
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::filter:\nError while filtering data.\n";
        throw Exception(errorMessage.str());
    }
}





void FitsService::doFinalTouchups()
{
    int status=Success;

    char *key="FILENAME";
    char *comment=0;
    std::string filename = *m_it;
    size_t pos = filename.find_last_of("/");
    if (pos == size_t(-1))
    {
        pos = filename.rfind("\\");
        if (pos == size_t(-1))
            pos = filename.rfind("!");
    }
    filename = filename.substr(pos+1);

    int hdutype;
    getNumHdus();
    for (int iext=1; iext<=m_numExtensions; ++iext)
    {
        if (fits_movabs_hdu(m_fptr, iext, &hdutype, &status))
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::doFinalTouchups:\n" << 
                "Cannot move to extension " << iext << " in the specified file.\n";
            report_error(status);
        }	

        if (fits_write_date(m_fptr, &status))
        {
            report_error(status);
        }

        if (fits_update_key(m_fptr, Tstring, key, 
            const_cast<char *>(filename.c_str()), comment, &status))
        {
            report_error(status);
        }

        if (fits_write_chksum(m_fptr, &status))
        {
            report_error(status);
        }
    }
}


void FitsService::closeFile()
{
    if (m_fptr)
    {
        if (m_fptr->Fptr->open_count > 0)
        {
            int status=Success;

            if (m_mode == Write)
                doFinalTouchups();

            if (fits_close_file(m_fptr, &status))
            {
                std::ostringstream errorMessage;
                errorMessage << "FitsService::closeFile:\n" << 
                    "Error while closing file.\n";
                report_error(status);
            }
        }

        m_fptr=0;
    }
}


void FitsService::makePrimaryCurrent() const
{
    int status = Success;
    int hdutype;

    if (fits_movabs_hdu(m_fptr, 1, &hdutype, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::makePrimaryCurrent:\n" << 
            "Cannot move to primary HDU in the specified file.\n";
        throw Exception(errorMessage.str());
    }	

    m_currentExtension = primary;
}


void FitsService::makeCurrent(const std::string &extname) const
{
    int status = Success;
    if (fits_movnam_hdu(m_fptr, ANY_HDU, const_cast<char *>(extname.c_str()), 0, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::makeCurrent:\n" <<
            extname << " does not exist in the specified file.\n";
        throw Exception(errorMessage.str());
    }	

    m_currentExtension = extname;
    toUpper(m_currentExtension);
}


// Reset pointer to first filename
void FitsService::resetFilePointer()
{
    m_it = m_filename.begin();
}


int FitsService::columnNumber(const std::string &columnName) const
{
    int status=Success;
    int colnum;
    std::string message;
    if (fits_get_colnum(m_fptr, CASEINSEN, const_cast<char *>(columnName.c_str()), &colnum, &status))
    {
        if (status == COL_NOT_FOUND)
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::columnNumber:\nColumn " << columnName <<
                " not found.\n";
            throw Exception(errorMessage.str());
        }

        else
        {
            getErrorMessage(status, message);
            std::ostringstream errorMessage;
            errorMessage << "FitsService::columnNumber:\n" << message << ".\n";
            throw Exception(errorMessage.str());
        }
    }

    return colnum;
}


FitsService::FitsValueType FitsService::columnType(int colnum, long &repeat) const
{
    long  width=0L;
    int   datatype;
    int   status=Success;
    if (fits_get_coltype(m_fptr, colnum, &datatype, &repeat, &width, &status))
    {
        report_error(status);    
    }

    return FitsValueType(datatype);
}


char FitsService::keyType(const std::string &value) const
{
    char  type;
    int   status=Success;
    if (fits_get_keytype(const_cast<char *>(value.c_str()), &type, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::keyType:\n" <<
            "Error while retrieving type for keyValue: " << value << ".\n";
        throw Exception(errorMessage.str());
    }

    return type;
}


bool FitsService::keyClassOK(const std::string &key) const
{
    int keyClass = fits_get_keyclass(const_cast<char*>(key.c_str()));

    if ((keyClass != TYP_STRUC_KEY)  && 
        (keyClass != TYP_HDUID_KEY) &&
        (keyClass != TYP_UNIT_KEY))
        return true;
#ifdef FOO
    if ((keyClass == TYP_USER_KEY)  || 
        (keyClass == TYP_WCS_KEY)   ||
        (keyClass == TYP_CMPRS_KEY) ||
        (keyClass == TYP_CKSUM_KEY) ||
        (keyClass == TYP_REFSYS_KEY))
        return true;
#endif

    return false;
}


long FitsService::variableColumnType(int colnum, long rownum) const
{
    long  offset=0L;
    long  repeat;
    int   status=Success;
    if (fits_read_descript(m_fptr, colnum, rownum, &repeat, &offset, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::variableColumnType:\n" <<
            "Error while retrieving information for the variable " <<
            "length column.\n";
        throw Exception(errorMessage.str());
    }

    return repeat;
}


long FitsService::rowsToRead(long firstRow, long nrows) const
{
    std::map<std::string, long>::const_iterator it = m_numRows.find(m_currentExtension);
    if (it != m_numRows.end())
    {
        long numAvailableRows = (*it).second;
        if (firstRow <= numAvailableRows)
        {
            long temp = ((nrows == -1) ? numAvailableRows : nrows);
            return (((temp+firstRow-1) <= numAvailableRows) ? temp : numAvailableRows-firstRow+1);
        }

        else
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::rowsToRead:\nAttempt to read data " <<
                " outside of range.\n";
            throw Exception(errorMessage.str());
        }
    }

    else
    {
        std::ostringstream errorMessage;
        errorMessage << "RootService::rowsToRead:\nError while searching for " <<
            m_currentExtension << ".\n";
        throw Exception(errorMessage.str());
    }
}


void FitsService::modifyVectorLength(int colnum, long len) const
{
    int status=Success;
    if (fits_modify_vector_len(m_fptr, colnum, len, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::modifyVectorLength:\n" <<
            "Error while modifying length of column#: " << colnum << ".\n";
        throw Exception(errorMessage.str());
    }
}


#if 0
void FitsService::copyHeader()
{
    int status=Success;

    fitsfile *fptr;
    openFile(m_fptr->Fptr->filename, &fptr);
    status = fits_movnam_hdu(fptr, ANY_HDU, 
        const_cast<char *>(m_currentExtension.c_str()), 0, &status);


    if (fits_copy_header(fptr, m_fptr, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::copyHeader:\n" <<
            "Error while making a copy of the specified header.\n";
        throw Exception(errorMessage.str());
    }
}


void FitsService::copyData(const std::string &originalName, const std::string &newName)
{
    int status=Success;
    int colnum = columnNumber(originalName);

    if (fits_copy_col(m_fptr, m_fptr, colnum, 999, 1, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::copyData:\n" <<
            "Error while making a copy of the data.\n";
        throw Exception(errorMessage.str());
    }

    int ncols=0;
    if (fits_get_num_cols(m_fptr, &ncols, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::copyData:\n" <<
            "Error while reading number of columns.\n";
        throw Exception(errorMessage.str());
    }

    std::ostringstream os;
    os << "TTYPE" << ncols;
    std::string key = os.str();
    std::string comment = "&";
    if (fits_modify_key_str(m_fptr, const_cast<char *>(key.c_str()), 
        const_cast<char *>(newName.c_str()), 
        const_cast<char *>(comment.c_str()), &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::copyData:\n" <<
            "Error while modifying key name.\n";
        throw Exception(errorMessage.str());
    }

    fits_set_hdustruc(m_fptr, &status);
}
#endif

void FitsService::deleteKey(const std::string &key)
{
    int status=Success;

    if (fits_delete_key(m_fptr, const_cast<char *>(key.c_str()), &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::deleteKey:\n" <<
            "Error while deleting key " << key << ".\n";
        throw Exception(errorMessage.str());
    }
}


void FitsService::deleteData(const std::string &key)
{
    int status=Success;

    if (fits_delete_col(m_fptr, columnNumber(key), &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::deleteData:\n" <<
            "Error while deleting column " << key << ".\n";
        throw Exception(errorMessage.str());
    }
}


void FitsService::writeHistory(const std::string &history) const
{
    int status=Success;

    if (fits_write_history(m_fptr, const_cast<char *>(history.c_str()), &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::writeHistory:\n" <<
            "Error while writing history.\n";
        throw Exception(errorMessage.str());
    }
}


void FitsService::writeComment(const std::string &comment)
{
    int status=Success;

    if (fits_write_comment(m_fptr, const_cast<char *>(comment.c_str()), &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::writeComment:\n" <<
            "Error while writing comment.\n";
        throw Exception(errorMessage.str());
    }
}


void FitsService::read(const std::string &key, std::vector<std::string> &data, long firstRow, 
                       long nrows) const 
{
    readData<std::string>(key, data, firstRow, nrows);
}


void FitsService::read(const std::string &key, std::vector<double> &data, long firstRow, 
                       long nrows) const 
{
    readData<double>(key, data, firstRow, nrows);
}


void FitsService::read(const std::string &key, std::vector<float> &data, long firstRow, 
                       long nrows) const 
{
    readData<float>(key, data, firstRow, nrows);
}


void FitsService::read(const std::string &key, std::vector<long> &data, long firstRow, 
                       long nrows) const 
{
    readData<long>(key, data, firstRow, nrows);
}


void FitsService::read(const std::string &key, std::vector<int> &data, long firstRow, 
                       long nrows) const 
{
    readData<int>(key, data, firstRow, nrows);
}


void FitsService::read(const std::string &key, std::vector<short> &data, long firstRow, 
                       long nrows) const 
{
    readData<short>(key, data, firstRow, nrows);
}




void FitsService::readImage(long firstElem, long nelements, std::vector<double> &data) const
{
    readImage<double>(firstElem, nelements, data);
}


void FitsService::readImage(long firstElem, long nelements, std::vector<float> &data) const
{
    readImage<float>(firstElem, nelements, data);
}


void FitsService::readImage(long firstElem, long nelements, std::vector<unsigned long> &data) const
{
    readImage<unsigned long>(firstElem, nelements, data);
}


void FitsService::readImage(long firstElem, long nelements, std::vector<long> &data) const
{
    readImage<long>(firstElem, nelements, data);
}


void FitsService::readImage(long firstElem, long nelements, std::vector<char> &data) const
{
    readImage<char>(firstElem, nelements, data);
}


void FitsService::readImage(long firstElem, long nelements, std::vector<short> &data) const
{
    readImage<short>(firstElem, nelements, data);
}


void FitsService::readImage(long firstElem, long nelements, std::vector<unsigned short> &data) const
{
    readImage<unsigned short>(firstElem, nelements, data);
}



void FitsService::readImage(const long *fpixel, const long *lpixel, const long *inc, 
                            std::vector<double> &data) const
{
    readImage<double>(fpixel, lpixel, inc, data);
}


void FitsService::readImage(const long *fpixel, const long *lpixel, const long *inc, 
                            std::vector<float> &data) const
{
    readImage<float>(fpixel, lpixel, inc, data);
}


void FitsService::readImage(const long *fpixel, const long *lpixel, const long *inc, 
                            std::vector<long> &data) const
{
    readImage<long>(fpixel, lpixel, inc, data);
}


void FitsService::readImage(const long *fpixel, const long *lpixel, const long *inc, 
                            std::vector<int> &data) const
{
    readImage<int>(fpixel, lpixel, inc, data);
}


void FitsService::readImage(const long *fpixel, const long *lpixel, const long *inc, 
                            std::vector<short> &data) const
{
    readImage<short>(fpixel, lpixel, inc, data);
}




void FitsService::readImageHeader( int &bitpix, int &naxis,
                                  std::vector<long> &naxes, int &extend, 
                                  double &zero, double &scale) const
{
    int status=Success;

    naxis = 0;
    if (fits_get_img_dim(m_fptr, &naxis, &status))  
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::readImageHeader:\nError while reading nexis keyword.\n";
        throw Exception(errorMessage.str());
    }

    long *temp = new long[naxis];
    int simple = 0;
    bitpix = 0;
    extend = 0;
    long pcount=0;
    long gcount=0;
    if (fits_read_imghdr(m_fptr, MAXDIM, &simple, &bitpix, &naxis, temp, &pcount, &gcount,
        &extend, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::readKey:\nError while reading image header.\n";
        throw Exception(errorMessage.str());
    }

    resetBitpix(bitpix);

    for (int i=0; i<naxis; ++i)
        naxes[i] = temp[i];
    delete []temp;

    // get scale and zero
    if (fits_read_key_dbl(m_fptr, "BSCALE", &scale, 0, &status))
    {
        scale = 1.;
        status = Success;
    }

    if (fits_read_key_dbl(m_fptr, "BZERO", &zero, 0, &status))
    {
        zero = 0.;
        status = Success;
    }
}


void FitsService::writeImageHeader( int &bitpix, int &naxis,
                                   const std::vector<long> &naxes, bool &extend, double &zero, double &scale) const
{
    int status=Success;

    resetBitpixToWrite(bitpix);

    size_t  sz=naxes.size();
    long *temp = new long[sz];;
    for (size_t i=0; i<sz; ++i)
        temp[i] = naxes[i];
    std::string comment="";

    if (fits_write_imghdr(m_fptr, bitpix, naxis, temp, &status))
    {
        if (status == HEADER_NOT_EMPTY)
        {
            status = Success;
            writeKey("BITPIX", bitpix, comment);
            writeKey("NAXIS", naxis, comment);
            std::ostringstream os;
            int i=0;

            for (i=0; i<naxis; ++i)
            {
                os.str("");
                os << "NAXIS" << i+1;
                writeKey(const_cast<char *>(os.str().c_str()), naxes[i], comment);
            }
        }

        else
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::writeImageHeader:\nError while writing " <<
                " image header.\n";
            throw Exception(errorMessage.str());
        }
    }

    static bool simple=true;
    writeKey("SIMPLE", simple, comment);
    writeKey("EXTEND", extend, comment);
    writeKey("BZERO", zero, comment);
    writeKey("BSCALE", scale, comment);

    fits_set_hdustruc(m_fptr, &status);
    fits_flush_file(m_fptr, &status);
}


void FitsService::resetBitpixToWrite(int &bitpix) const
{
    switch(bitpix)
    {
    case table::Ibyte:
        bitpix = Ibyte;
        break;

    case table::Ishort:
        bitpix = Ishort;
        break;

    case table::Ilong:
        bitpix = Ilong;
        break;

    case table::Ifloat:
        bitpix = Ifloat;
        break;

    case table::Idouble:
        bitpix = Idouble;
        break;

    case table::Iushort:
        bitpix = Iushort;
        break;

    case table::Iulong:
        bitpix = Iulong;
        break;

    default:
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::resetBitpixToWrite:\nInvalid bitpix value.\n";
            throw Exception(errorMessage.str());
        }
    }
}


void FitsService::resetBitpix(int &bitpix) const
{
    switch(bitpix)
    {
    case Ibyte:
        bitpix = table::Ibyte;
        break;

    case Ishort:
        bitpix = table::Ishort;
        break;

    case Ilong:
        bitpix = table::Ilong;
        break;

    case Ifloat:
        bitpix = table::Ifloat;
        break;

    case Idouble:
        bitpix = table::Idouble;
        break;

    case Iushort:
        bitpix = table::Iushort;
        break;

    case Iulong:
        bitpix = table::Iulong;
        break;

    default:
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::resetBitpix:\nInvalid bitpix value.\n";
            throw Exception(errorMessage.str());
        }
    }
}


void FitsService::readKey(int keynum, std::string &key, std::string &value, std::string &comment) const
{
    char keyname[FLEN_KEYWORD];
    char val[FLEN_VALUE];
    char comm[FLEN_COMMENT];

    int status=Success;
    if (fits_read_keyn(m_fptr, keynum, keyname, val, comm, &status)) 
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::readKey:\nError while reading key#: " <<
            keynum << ".\n";
        throw Exception(errorMessage.str());
    }

    key = std::string(keyname);
    if (key == "HISTORY")
    {
        value = std::string(comm);
        comment = std::string("");
    }

    else
    {
        value = std::string(val);
        comment = std::string(comm);
    }
}


void FitsService::readKey(const std::string &key, std::string &value, std::string &comment) const
{
    readStringKey(key, value, comment);
}


void FitsService::readKey(const std::string &key, float &value, std::string &comment) const
{
    readKey<float>(key, Tfloat, value, comment);
}


void FitsService::readKey(const std::string &key, long &value, std::string &comment) const
{
    readKey<long>(key, Tlong, value, comment);
}


void FitsService::readKey(const std::string &key, int &value, std::string &comment) const
{
    readKey<int>(key, Tint, value, comment);
}


void FitsService::readKey(const std::string &key, bool &value, std::string &comment) const
{
    readKey<bool>(key, Tlogical, value, comment);
}


void FitsService::readStringKey(const std::string &key, std::string &value, std::string &comment) const
{
    int status=Success;

    char comm[FLEN_COMMENT];
    char val[FLEN_VALUE];

    if (fits_read_key(m_fptr, Tstring, const_cast<char *>(key.c_str()), val, comm, &status))
    {
        std::ostringstream errorMessage;
        errorMessage << "FitsService::readStringKey:\nError while reading key: " <<
            key << ".\n";
        throw Exception(errorMessage.str());
    }

    value   = val;
    comment = comm;
}


void FitsService::write(const std::string &key, const std::vector<std::string> &data, long firstRow) const
{
    writeData<>(key, data, firstRow);
}


void FitsService::write(const std::string &key, const std::vector<double> &data, long firstRow) const
{
    writeData<double>(key, data, firstRow);
}


void FitsService::write(const std::string &key, const std::vector<float> &data, long firstRow) const
{
    writeData<float>(key, data, firstRow);
}


void FitsService::write(const std::string &key, const std::vector<long> &data, long firstRow) const
{
    writeData<long>(key, data, firstRow);
}


void FitsService::write(const std::string &key, const std::vector<int> &data, long firstRow) const
{
    writeData<int>(key, data, firstRow);
}


void FitsService::write(const std::string &key, const std::vector<short> &data, long firstRow) const
{
    writeData<short>(key, data, firstRow);
}






void FitsService::writeImage(long firstElem, long nelements, const std::vector<double> &data) const
{
    writeImage<double>(Tdouble, firstElem, nelements, data);
}


void FitsService::writeImage(long firstElem, long nelements, const std::vector<float> &data) const
{
    writeImage<float>(Tfloat, firstElem, nelements, data);
}


void FitsService::writeImage(long firstElem, long nelements, const std::vector<unsigned long> &data) const
{
    writeImage<unsigned long>(Tulong, firstElem, nelements, data);
}


void FitsService::writeImage(long firstElem, long nelements, const std::vector<long> &data) const
{
    writeImage<long>(Tlong, firstElem, nelements, data);
}


void FitsService::writeImage(long firstElem, long nelements, const std::vector<char> &data) const
{
    writeImage<char>(Tbyte, firstElem, nelements, data);
}


void FitsService::writeImage(long firstElem, long nelements, const std::vector<unsigned short> &data) const
{
    writeImage<unsigned short>(Tushort, firstElem, nelements, data);
}


void FitsService::writeImage(long firstElem, long nelements, const std::vector<short> &data) const
{
    writeImage<short>(Tshort, firstElem, nelements, data);
}




void FitsService::writeImage(const long *fpixel, const long *lpixel, 
                             const std::vector<double> &data) const
{
    writeImage<double>(Tdouble, fpixel, lpixel, data);
}


void FitsService::writeImage(const long *fpixel, const long *lpixel, 
                             const std::vector<float> &data) const
{
    writeImage<float>(Tfloat, fpixel, lpixel, data);
}


void FitsService::writeImage(const long *fpixel, const long *lpixel, 
                             const std::vector<long> &data) const
{
    writeImage<long>(Tlong, fpixel, lpixel, data);
}


//void FitsService::writeImage(const long *fpixel, const long *lpixel, 
//const std::vector<int> &data) const
//{
//    writeImage<int>(Tint, fpixel, lpixel, data);
//}


void FitsService::writeImage(const long *fpixel, const long *lpixel, 
                             const std::vector<short> &data) const
{
    writeImage<short>(Tshort, fpixel, lpixel, data);
}


// void FitsService::writeImage(const long *fpixel, const long *lpixel, 
// const std::vector<int> &data) const
// {
//    writeImage<int>(Tint, fpixel, lpixel, data);
//}




void FitsService::writeKey(const std::string &key, const std::string &value, 
                           const std::string &comment) const
{
    writeStringKey(key, value, comment);
}


void FitsService::writeKey(const std::string &key, double value, 
                           const std::string &comment) const
{
    writeKey<double>(key, Tdouble, value, comment);
}


void FitsService::writeKey(const std::string &key, float value, 
                           const std::string &comment) const
{
    writeKey<float>(key, Tfloat, value, comment);
}


void FitsService::writeKey(const std::string &key, long value, 
                           const std::string &comment) const
{
    writeKey<long>(key, Tlong, value, comment);
}


void FitsService::writeKey(const std::string &key, int value, 
                           const std::string &comment) const
{
    writeKey<int>(key, Tint, value, comment);
}


void FitsService::writeKey(const std::string &key, short value, 
                           const std::string &comment) const
{
    writeKey<short>(key, Tshort, value, comment);
}


void FitsService::writeKey(const std::string &key, bool value, 
                           const std::string &comment) const
{
    writeKey<bool>(key, Tlogical, value, comment);
}


void FitsService::writeStringKey(const std::string &key, const std::string &value, 
                                 const std::string &comment) const
{
    if (value.size() > 0)
    {
        int status=Success;

        char *comm=0;
        if (comment.size() > 0)
        {
            comm = new char[comment.size()];
            strcpy(comm, comment.c_str());
        }

        if (fits_update_key(m_fptr, Tstring, const_cast<char *>(key.c_str()), 
            const_cast<char *>(value.c_str()), 
            comm, &status))
        {
            std::ostringstream errorMessage;
            errorMessage << "FitsService::writeStringKey:\n" << 
                "Error while writing key " << key << ".\n";
            throw Exception(errorMessage.str());
        }

        if (comm)
            delete comm;
    }
}


void FitsService::clearErrorMsg()
{
    fits_clear_errmsg();
}


// Error Messages
void FitsService::getErrorMessage(int status, std::string &message) const
{
    char msg[FLEN_ERRMSG];
    fits_get_errstatus(status, msg);
    std::cout << "FitsService::getErrorMessage: "
        << msg << "  "
        << status << std::endl;
    message = std::string(msg);
}

void FitsService::report_error(int status)const
{
    if(status==0) return;
    std::stringstream msg; msg << "fits error #: " << status;
    static char buffer[240]; 
    while ( fits_read_errmsg(buffer)!=0 ) msg << "\n" << buffer;
    std::cerr << msg.str() << std::endl;
    const_cast<FitsService*>(this)->m_fptr=0; // prevent code running 
    throw Exception(msg.str());
}
