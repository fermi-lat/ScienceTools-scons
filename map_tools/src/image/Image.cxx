/** @file Image.cxx
    @brief Implementation of BaseImage 

     @author Toby Burnett
     Code orginally written by Riener Rohlfs

     $Header$
*/
#include "Image.h"

//_____________________________________________________________________________

BaseImage *  BaseImage::readImage(const std::string & file_Name, const std::string & name,  
                        unsigned int cycle, VirtualIO::FMode mode)
{
// reads an image from a file 

    IOElement * element = IOElement::readIOElement(file_Name, name, cycle, mode ); 

   BaseImage * image = dynamic_cast<BaseImage *>(element);
   if (image == 0)
      delete element;

   return image;
}

//_____________________________________________________________________________
unsigned int   BaseImage::pixelCount()const
{
    unsigned int count = 1;
    for(std::vector<long>::const_iterator it = m_axisSize.begin(); it != m_axisSize.end(); ++ it){
        count *= *it;
    }
    return count;
}
