/** @file IOElement.h

*/
#ifndef IOELEMENT_H
#define IOELEMENT_H


#include "Header.h"
#include "VirtualIO.h"

//_____________________________________________________________________________
/** @class IOElement
    @brief file operations

   The IOElement class is the base class to save data structures into a 
   A file. 

   The IOElement class does not have data itself, but a header to store 
   header information and always a name. Most of the time derived classes 
   are used to write data into a file and read data from a file. For example 
   tables or images.

   To create a new element either in memory or in a 
   file the constructors can be used. To create an element in memory 
   that reads its data and header from a file the global function
   TFRead has to be used. Beside IOElements this function can read 
   also any derived class from IOElement.

   All changes of a IOElement or its derived classes are done only in 
   memory and not automatically updated in the associated file. To 
   save the changes the member function saveElement() has to be called.
*/
class IOElement:  public Header
{

public:
    IOElement(){};

    /**
     A new element is created. It is associated with an element in a 
     file. name is the id of the new element. 
     fileName is the file name of the  file with this new 
     element. The file  may already exist or will be created. Other objects 
     with the same name can exist without problem in the same file. Each object 
     with the same name has a unique cycle number. Use getCycle() to ask for 
     this cycle number. Use the function readIOElement() to read and update an already 
     existing element from a  file.
    */
    IOElement(const std::string & name, const  std::string & fileName);

    IOElement(const std::string & filename);


    virtual ~IOElement();
    virtual bool isImage()const=0;

    void           setIO(VirtualIO * io)   {m_fio = io;}
    void           setFileAccess(VirtualIO::FMode mode) {m_fileAccess = mode;}

    virtual  bool         isFileConnected() const {return m_fio != NULL && m_fio->isOpen();}
    virtual  std::string  getFileName() const     {return m_fio ? m_fio->getFileName() : "";}
    virtual  int          getCycle() const        {return m_fio ? m_fio->getCycle() : 0;}
    virtual std::string  getName()const { return m_name; }


    virtual  void         closeElement();

    /**
    Updates the element in the file with the data of this element in
    memory. 
    @param fileName If fileName is defined the old file is closed and the element
    is written into this new file named fileName.

    This function without any parameter has to be used to update the
    file with any change of the element in memory.
    */
    virtual  int          saveElement(const std::string & fileName = "");

    virtual  int          deleteElement(bool updateMemory = false);

    /** @brief read an IOElement from a file

     This static function reads a IOElement or a derived object from a 
      file, depending on the file extension 
     and returns its pointer. The returned element has to be deleted by the 
     calling function.
    
     @param fileName: defines the file. Absolute path, relative path or just a 
           filename are accepted names.
           If mode == kFRead the file has to have read permission. If mode ==
           kFReadWrite it also has to have write permission.
     @param name: Is the identifier of the element. If more than one element with
           the same name exist in the file the first is returned if 
           cycle is set to the default value of 0. Otherwise the element with
           the specified cycle number is looked for in the file.
     @param cycle: If it is not the default value of 0 only the element with the 
           specified  cycle number will be read.
     @param mode: has to be either kFRead or kFReadWrite. 
           kfRead: The file and the required element must already exist. The
                   element cannot be updated.
           kfReadWrite: The file and the required element must already exist.
                   The element can be updated with the member function 
                   SaveElement()
     If there is an error during opening or reading the file or if the required
     element is not in the file the functions will throw an exception  or 
     will return a NULL pointer.
    
    */
    static IOElement *  readIOElement(const std::string & fileName,const std::string & name,  
        unsigned int cycle = 0, VirtualIO::FMode mode = VirtualIO::kFRead);

protected:
    VirtualIO * fio(){ return m_fio;}
private:
    virtual  void           updateMemory() {}

    virtual  void           newFile(const std::string & fileName);

    VirtualIO  * m_fio;        //!interface to the file
    VirtualIO::FMode        m_fileAccess;  //!access to the file read, readwrite

    std::string m_name;
};

#endif
