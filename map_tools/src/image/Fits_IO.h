/**  @file  Fits_IO.h
    @brief definition of class Fits_IO

     @author Toby Burnett
     Code orginally written by Riener Rohlfs

     $Header$
*/
#ifndef ROOT_TFFitsIO_H
#define ROOT_TFFitsIO_H


#include "VirtualIO.h"

class TClass;
/** @class Fits_IO
    @brief FITS-specific implementation of VirtualIO interface

    */
class Fits_IO : public VirtualIO
{

public:
   Fits_IO() {}
   Fits_IO( IOElement * element, const std::string & fileName);
   Fits_IO( IOElement * element, void * fptr, int cycle);
 
   ~Fits_IO();

   /**
     tries to open an element in a FITS file and converts it into IOElement
     or one of its derived classes.
     In case there is a problem  0 is 
     returned.
   */
   static   IOElement *  read(const std::string & fileName, const std::string & name, 
			      int cycle = 0, VirtualIO::FMode mode = Read);

   /// return true if an element is successfull open
   virtual  bool         isOpen();
    /**
     @return the filename of an element, or an empty string 
     if the element is not open or in case of a cfitsio error
    */
   virtual  const std::string    getFileName();
   
   /**
     @return the cycle number == HDU number. 
     First element in FITS file (primary header) has cycle 1
   */
   virtual  int          getCycle();

   virtual  void         createElement();

   /**
    deletes this HDU and deletes the file if if is the last HDU
    Note: cfitsio will replace the first HDU == primary array by
    an empty one if this primary array should be deleted.
   */
   virtual  int          deleteElement();
   
   /** @brief save the element to the current fits file
   */
   virtual  void          saveElement();

   static void report_error(int status);
private:
    //! helper functions
    void writeFitsHeader();
    //! helper to attach fits headers to an element
    void readFitsHeader();

   void  * m_fptr;
   int   m_cycle;

};



#endif // ROOT_TFVirtualIO
