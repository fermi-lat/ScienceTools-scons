/** @file IOElement.cxx
    @brief implementaion of IOElement
 
     @author Toby Burnett
     Code orginally written by Riener Rohlfs

     $Header$
*/

#include "IOElement.h"
#include "Fits_IO.h"

#include <stdexcept>
//_____________________________________________________________________________
IOElement *  IOElement::readIOElement(const std::string & fileName, const std::string & name,  
                                      unsigned int cycle, VirtualIO::FMode mode)
{
    return Fits_IO::read(fileName, name, cycle, mode);
}

//_____________________________________________________________________________
IOElement::IOElement(const std::string & name)
: m_fio(0)
, m_fileAccess(VirtualIO::Undefined)
, m_name(name) 
{
}

//_____________________________________________________________________________
IOElement::IOElement(const std::string & name, const std::string & fileName) 
: m_name(name) 
{

    newFile(fileName);
}
//_____________________________________________________________________________
IOElement::~IOElement()
{
    // The associated file is closed, the element is NOT updated. Use
    // SaveElement() to update the changes in the file.

    closeElement();
}
//_____________________________________________________________________________
void IOElement::newFile(const std::string & fileName)
{
    // protected function to create a new element in a file 
    m_fio = new Fits_IO(this, fileName);

    if (!m_fio->isOpen())
        closeElement();
    else
        m_fileAccess    = VirtualIO::ReadWrite;
}
//_____________________________________________________________________________
void IOElement::saveElement(const std::string & fileName)
{

    if (!fileName.empty()) {
        // read everything from the old file and close it
        updateMemory();
        closeElement();

        newFile(fileName);

        if (!m_fio || !m_fio->isOpen())
            return ;

        m_fio->createElement();
    }

    if (!m_fio || !m_fio->isOpen() )
        return;

    if (m_fileAccess == VirtualIO::Read) {
        throw std::runtime_error(std::string("IOElement::saveElement--wrong access mode to save file")+fileName);
    }else if (m_fileAccess == VirtualIO::ReadWrite){
         m_fio->saveElement();
    }

    return;
}
//_____________________________________________________________________________
void IOElement::closeElement()
{
    // Closes the element in the associated file. It does not update the
    // element in the file. The element still exist in memory after
    // a call of this function.

    delete m_fio;
    m_fio    = NULL;

    m_fileAccess    = VirtualIO::Undefined;
}
//_____________________________________________________________________________
int IOElement::deleteElement(bool update)
{

    if (m_fio == NULL) return 0;

    if (update) updateMemory();

    int err = 0;
    if (m_fileAccess == VirtualIO::Read) {
        throw std::runtime_error("cannon delete element: wrong file access");
        return -1;
    }
    else if (m_fileAccess == VirtualIO::ReadWrite)
        err = m_fio->deleteElement();

    closeElement();

    return err;
}
