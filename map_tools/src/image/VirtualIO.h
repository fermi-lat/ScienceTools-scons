/** @file VirtualIO.h


*/
#ifndef VIRTUALIO_H
#define VIRTUALIO_H

#include <string>
#include <map>

class IOElement;
class BaseCol;
class ColList;

//_____________________________________________________________________________

class VirtualIO
{
public:
    typedef enum {kFUndefined = 0, kFRead = 1, kFReadWrite = 2} FMode;

   VirtualIO( IOElement * element=0);
 
   virtual ~VirtualIO();

   virtual  bool         isOpen() = 0;
   virtual  const std::string   getFileName() = 0;
   virtual  int          getCycle() = 0;

   virtual  void         createElement() = 0;
   virtual  int          deleteElement() = 0;
   virtual  int          saveElement()=0;

   IOElement* element()const{return m_element;}


protected:
  IOElement * m_element;  // the element associated with this IOfile
};


#endif 
