/** @file VirtualIO.h
    @brief definition of VirtualIO class

     @author Toby Burnett
     Code orginally written by Riener Rohlfs

     $Header$

*/
#ifndef VIRTUALIO_H
#define VIRTUALIO_H

#include <string>
#include <map>

class IOElement;
class BaseCol;
class ColList;

//_____________________________________________________________________________
/** @class VirtualIO
    @brief abstract base class for IO; contains an IOElement

    */
class VirtualIO
{
public:
    typedef enum {Undefined = 0, Read = 1, ReadWrite = 2} FMode;

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
