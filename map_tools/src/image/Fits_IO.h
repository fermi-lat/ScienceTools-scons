/**  @file  Fits_IO.h
*/
#ifndef ROOT_TFFitsIO_H
#define ROOT_TFFitsIO_H


#include "VirtualIO.h"
class TClass;

class Fits_IO : public VirtualIO
{

public:
   Fits_IO() {}
   Fits_IO( IOElement * element, const std::string & fileName);
   Fits_IO( IOElement * element, void * fptr, int cycle);
 
   ~Fits_IO();

   static   IOElement *  read(const std::string & fileName, const std::string & name, 
       int cycle = 0, VirtualIO::FMode mode = kFRead, 
                                  TClass * classType = NULL);

   virtual  bool         isOpen();
   virtual  const std::string    getFileName();
   virtual  int          getCycle();

   virtual  void         createElement();
   virtual  int          deleteElement();
   virtual  int          saveElement(int compLevel = -1);

   static void report_error(int status);
private:
   void  * m_fptr;
   int   m_cycle;

};



#endif // ROOT_TFVirtualIO
